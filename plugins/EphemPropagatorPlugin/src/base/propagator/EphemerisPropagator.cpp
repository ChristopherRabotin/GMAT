//$Id$
//------------------------------------------------------------------------------
//                             EphemerisPropagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: Mar 26, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the EphemerisPropagator class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <cstdio>                   // for sprintf
#include "EphemerisPropagator.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"


//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------

/// EphemerisPropagator parameter labels
const std::string
EphemerisPropagator::PARAMETER_TEXT[EphemerisPropagatorParamCount - PropagatorParamCount] =
{
      "StepSize",       // EPHEM_STEP_SIZE
      "CentralBody",    // EPHEM_CENTRAL_BODY
      "EpochFormat",    // EPHEM_EPOCH_FORMAT
      "StartEpoch",     // EPHEM_START_EPOCH
      "StartOptions"    // EPHEM_START_OPTIONS
};

/// EphemerisPropagator parameter types
const Gmat::ParameterType
EphemerisPropagator::PARAMETER_TYPE[EphemerisPropagatorParamCount - PropagatorParamCount] =
{
      Gmat::REAL_TYPE,        // EPHEM_STEP_SIZE
      Gmat::OBJECT_TYPE,      // EPHEM_CENTRAL_BODY
      Gmat::STRING_TYPE,      // EPHEM_EPOCH_FORMAT
      Gmat::STRING_TYPE,      // EPHEM_START_EPOCH
      Gmat::STRINGARRAY_TYPE  // EPHEM_START_OPTIONS
};


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EphemerisPropagator(const std::string & typeStr, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param typeStr Script description for the propagator type
 * @param name Name of the constructed object
 */
//------------------------------------------------------------------------------
EphemerisPropagator::EphemerisPropagator(const std::string & typeStr,
      const std::string & name) :
   Propagator           (typeStr, name),
   ephemStep            (300.0),
   epochFormat          ("A1ModJulian"),
   initialEpoch         (-987654321.0),
   currentEpoch         (-987654321.0),
   timeFromEpoch        (0.0),
   ephemStart           (-987654321.0),
   ephemEnd             (987654321.0),
   psm                  (NULL),
   state                (NULL),
   j2kState             (NULL),
   stepTaken            (0.0),
   startEpochSource     (FROM_SCRIPT),
   stepDirection        (1.0),
   solarSystem          (NULL)
{
   objectTypeNames.push_back("EphemerisPropagator");
   parameterCount = EphemerisPropagatorParamCount;

   std::stringstream epochString("");
   epochString << GmatTimeConstants::MJD_OF_J2000;
   startEpoch = epochString.str();
   startOptions.push_back("FromSpacecraft");
//   startOptions.push_back("EphemStart");
}


//------------------------------------------------------------------------------
// ~EphemerisPropagator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemerisPropagator::~EphemerisPropagator()
{
   if (state != NULL)
      delete [] state;

   if (j2kState != NULL)
      delete [] j2kState;
}


//------------------------------------------------------------------------------
// EphemerisPropagator(const EphemerisPropagator & ep)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ep The Ephemeris propagator copied to the new one.
 */
//------------------------------------------------------------------------------
EphemerisPropagator::EphemerisPropagator(const EphemerisPropagator & ep) :
   Propagator              (ep),
   ephemStep               (ep.ephemStep),
   epochFormat             (ep.epochFormat),
   startEpoch              (ep.startEpoch),
   initialEpoch            (ep.initialEpoch),
   currentEpoch            (ep.currentEpoch),
   timeFromEpoch           (ep.timeFromEpoch),
   ephemStart              (ep.ephemStart),
   ephemEnd                (ep.ephemEnd),
   psm                     (NULL),
   state                   (NULL),
   j2kState                (NULL),
   stepTaken               (0.0),
   startOptions            (ep.startOptions),
   startEpochSource        (ep.startEpochSource),
   stepDirection           (ep.stepDirection),
   solarSystem             (NULL)
{
}


//------------------------------------------------------------------------------
// EphemerisPropagator& operator=(const EphemerisPropagator& ep)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ep The Ephemeris propagator supplying parameters for this one.
 *
 * @return This EphemerisPropagator configured to match ep.
 */
//------------------------------------------------------------------------------
EphemerisPropagator& EphemerisPropagator::operator=(
      const EphemerisPropagator& ep)
{
   if (this != &ep)
   {
      Propagator::operator=(ep);

      ephemStep     = ep.ephemStep;
      epochFormat   = ep.epochFormat;
      startEpoch    = ep.startEpoch;
      initialEpoch  = ep.initialEpoch;
      currentEpoch  = ep.currentEpoch;
      timeFromEpoch = ep.timeFromEpoch;
      ephemStart    = ep.ephemStart;
      ephemEnd      = ep.ephemEnd;
      psm           = NULL;
      if (state != NULL)
      {
         delete [] state;
         state = NULL;
      }
      if (j2kState != NULL)
      {
         delete [] j2kState;
         j2kState = NULL;
      }
      stepTaken        = 0.0;
      startOptions     = ep.startOptions;
      startEpochSource = ep.startEpochSource;
      stepDirection    = ep.stepDirection;
      solarSystem      = NULL;
   }

   return *this;
}


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
Integer EphemerisPropagator::GetParameterID(const std::string &str) const
{
   for (Integer i = PropagatorParamCount;
         i < EphemerisPropagatorParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - PropagatorParamCount])
         return i;
   }

   return Propagator::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetParameterText(const Integer id) const
{
   if (id >= PropagatorParamCount && id < EphemerisPropagatorParamCount)
      return PARAMETER_TEXT[id - PropagatorParamCount];
   return Propagator::GetParameterText(id);
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
std::string EphemerisPropagator::GetParameterUnit(const Integer id) const
{
   return Propagator::GetParameterUnit(id);
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
Gmat::ParameterType EphemerisPropagator::GetParameterType(
      const Integer id) const
{
   if (id >= PropagatorParamCount && id < EphemerisPropagatorParamCount)
      return PARAMETER_TYPE[id - PropagatorParamCount];
   return Propagator::GetParameterType(id);
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
std::string EphemerisPropagator::GetParameterTypeString(const Integer id) const
{
   if (id >= PropagatorParamCount && id < EphemerisPropagatorParamCount)
      return Propagator::PARAM_TYPE_STRING[GetParameterType(id)];
   return Propagator::GetParameterTypeString(id);
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
bool EphemerisPropagator::IsParameterReadOnly(const Integer id) const
{
   if ((id == INITIAL_STEP_SIZE) || (id == EPHEM_START_OPTIONS))
      return true;
   return Propagator::IsParameterReadOnly(id);
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
bool EphemerisPropagator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the Real parameters specific to the EphemerisPropagator
 *
 * @param id The ID for the parameter that is retrieved
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const Integer id) const
{
   if (id == EPHEM_STEP_SIZE)
   {
      return ephemStep;
   }

   return Propagator::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the Real parameters specific to the EphemerisPropagator
 *
 * @param id The ID for the parameter that is retrieved
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_SET_PROPAGATOR_PARAMETER
   	MessageInterface::ShowMessage("EphemProp %p setting ID %d to %lf\n", this,
      	   id, value);
   #endif
   if (id == EPHEM_STEP_SIZE)
   {
      if (value != 0.0)
         ephemStep = value;
      return ephemStep;
   }

   if (id == INITIAL_STEP_SIZE)
   {
//      if (value != 0.0)
//         ephemStep = value;

      if (value < 0.0)
         stepDirection = -1.0;
      else
         stepDirection = 1.0;
      // Let this fall through to Propagator, so no return here
   }

   #ifdef DEBUG_SET_PROPAGATOR_PARAMETER
      MessageInterface::ShowMessage("   ---> <%p> has ephem step %lf\n", this, ephemStep);
   #endif
   
   return Propagator::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves parameters specific to the EphemerisPropagator from a RealArray
 *
 * @param id The ID for the parameter that is retrieved
 * @param index The index of the parameter in the RealArray
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const Integer id,
      const Integer index) const
{
   return Propagator::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieves parameters specific to the EphemerisPropagator from a Real matrix
 *
 * @param id The ID for the parameter that is retrieved
 * @param row The row index of the parameter in the matrix
 * @param col The column index of the parameter in the matrix
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const Integer id, const Integer row,
      const Integer col) const
{
   return Propagator::GetRealParameter(id, row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves parameters specific to the EphemerisPropagator from a RealArray
 *
 * @param id The ID for the parameter that is retrieved
 * @param value The new value for the parameter
 * @param index The index of the parameter in the RealArray
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   return Propagator::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer row,
//       const Integer col)
//------------------------------------------------------------------------------
/**
 * Retrieves parameters specific to the EphemerisPropagator from a RealArray
 *
 * @param id The ID for the parameter that is retrieved
 * @param value The new value for the parameter
 * @param row The row index of the parameter in the matrix
 * @param col The column index of the parameter in the matrix
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const Integer id, const Real value,
      const Integer row, const Integer col)
{
   return Propagator::SetRealParameter(id, value, row, col);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter from then object
 *
 * @param label Script identifier for the parameter
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Real parameter
 *
 * @param label Script identifier for the parameter
 * @param value The new value
 *
 * @return The parameter's value at the end of the call
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const std::string &label,
      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter value from a RealArray
 *
 * @param label Script identifier for the parameter
 * @param index The index of the parameter in the array
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const std::string &label,
      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real EphemerisPropagator::SetRealParameter(const std::string &label,
//       const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a Real parameter value in a RealArray
 *
 * @param label Script identifier for the parameter
 * @param value The new value
 * @param index The index of the parameter in the array
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const std::string &label,
      const Real value, const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter value from a matrix of Reals
 *
 * @param label Script identifier for the parameter
 * @param row The row index of the parameter in the matrix
 * @param col The column index of the parameter in the matrix
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const std::string &label,
      const Integer row, const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value,
//       const Integer row, const Integer col))
//------------------------------------------------------------------------------
/**
 * Sets a Real parameter value in a matrix of Reals
 *
 * @param label Script identifier for the parameter
 * @param value The new value
 * @param row The row index of the parameter in the matrix
 * @param col The column index of the parameter in the matrix
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const std::string &label,
      const Real value, const Integer row, const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves string parameters
 *
 * @param id The id for the parameter
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string  EphemerisPropagator::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case EPHEM_CENTRAL_BODY:
         return centralBody;

      case EPHEM_EPOCH_FORMAT:
         return epochFormat;

      case EPHEM_START_EPOCH:
         return startEpoch;

      default:
         break;
   }

   return Propagator::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetStringParameter(const Integer id,
      const std::string &value)
{
   switch (id)
   {
      case EPHEM_CENTRAL_BODY:
         centralBody = value;
         return true;

      case EPHEM_EPOCH_FORMAT:
         if (TimeConverterUtil::IsValidTimeSystem(value))
         {
            epochFormat = value;   
         }
         else
         {
            char msg[512];
            std::string timeRepList;
            StringArray validReps = TimeConverterUtil::GetValidTimeRepresentations();
            for (UnsignedInt i = 0; i < validReps.size(); ++i)
            {
               if (i != 0)
                  timeRepList += ", ";
               timeRepList += validReps[i];
            }

            // The valid format list here should be retrieved from TimeconverterUtil,
            // once that code is refactored
            std::sprintf(msg, errorMessageFormat.c_str(), value.c_str(),
                  PARAMETER_TEXT[EPHEM_EPOCH_FORMAT - PropagatorParamCount].c_str(),
                  timeRepList.c_str());

            throw PropagatorException(msg);
         }
         return true;

      case EPHEM_START_EPOCH:
         startEpoch = value;
         if (startEpoch == "FromSpacecraft")
            startEpochSource = FROM_SPACECRAFT;
         // Not currently supported:
//         else if (startEpoch == "EphemStart")
//            startEpochSource = FROM_EPHEM;
         else
            startEpochSource = FROM_SCRIPT;
         return true;

      default:
         break;
   }

   return Propagator::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves string parameters from a StringArray
 *
 * @param id The id for the parameter
 * @param index Index into the array
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetStringParameter(const Integer id,
      const Integer index) const
{
   return Propagator::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets string parameters in a StringArray
 *
 * @param id The id for the parameter
 * @param value The new string value
 * @param index Index into the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   return Propagator::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves string parameters
 *
 * @param label The script name for the parameter
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetStringParameter(
      const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param label The script name for the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves string parameters from a StringArray
 *
 * @param label The script name for the parameter
 * @param index Index into the array
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets string parameters in a StringArray
 *
 * @param label The script name for the parameter
 * @param value The new string value
 * @param index Index into the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters
 *
 * @param id The ID of the parameter
 *
 * @return The corresponding StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetStringArrayParameter(
      const Integer id) const
{
   if (id == EPHEM_START_OPTIONS)
   {
      return startOptions;
   }
   return Propagator::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters from a vector of StringArrays
 *
 * @param id The ID of the parameter
 * @param index The index of the array in the vector of arrays
 *
 * @return The corresponding StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetStringArrayParameter(
      const Integer id, const Integer index) const
{
   return Propagator::GetStringArrayParameter(id, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters from a vector of StringArrays
 *
 * @param label The text label of the parameter
 *
 * @return The corresponding StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters from a vector of StringArrays
 *
 * @param label The text label of the parameter
 * @param index The index of the array in the vector of arrays
 *
 * @return The corresponding StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}

// Reference object code

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss Pointer to the solar system used in the modeling.
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetSolarSystem(SolarSystem *ss)
{
   if (ss == NULL)
      MessageInterface::ShowMessage("Setting NULL solar system on %s\n",
         instanceName.c_str());
   else
   {
      solarSystem = ss;

      // "Moon" no longer allowed
//      std::string bName = centralBody;
//      if (bName == "Moon")
//         bName = "Luna";
      propOrigin = solarSystem->GetBody(centralBody);

      if (propOrigin == NULL)
         throw PropagatorException(
            "Ephemeris propagator origin (" + centralBody +
            ") was not found in the solar system");

      // "Moon" no longer allowed
//      bName = j2kBodyName;
//      if (bName == "Moon")
//         bName = "Luna";
      j2kBody = solarSystem->GetBody(j2kBodyName);
   }
}


//------------------------------------------------------------------------------
// std::string  GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a reference object of the specified type
 *
 * @param type The type of the object
 *
 * @return The object's name
 */
//------------------------------------------------------------------------------
std::string  EphemerisPropagator::GetRefObjectName(
      const Gmat::ObjectType type) const
{
      return Propagator::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of reference objects of the specified type
 *
 * @param type The type of the objects
 *
 * @return The list of names
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   #ifdef DEBUG_REF_OBJ
      MessageInterface::ShowMessage("EphemerisPropagator::"
            "GetRefObjectNameArray(%d) called\n", type);
   #endif

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
      return propObjectNames;

   return Propagator::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Passes in the name of a reference object for use during initialization
 *
 * @param type The type of the object
 * @param name The object's name
 *
 * @return true if the name was set, false if not
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetRefObjectName(const Gmat::ObjectType type,
      const std::string &name)
{
   bool retval = false;

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
   {
      if (find(propObjectNames.begin(), propObjectNames.end(), name) ==
            propObjectNames.end())
         propObjectNames.push_back(name);

      retval = true;
   }

   return (Propagator::SetRefObjectName(type, name) || retval);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string &oldName,
//       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Changes the name of a previously set reference object.
 *
 * This method changes the name or list of names of reference objects.  It does
 * not change the actual objects themselves; that side of the renaming is
 * handled separately.
 *
 * @param type The type of object that is being renamed
 * @param oldName The object's name before the change
 * @param newName The name that now identifies the object
 *
 * @return true if a object name was changed, false if not
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::RenameRefObject(const Gmat::ObjectType type,
      const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
   {
      for (UnsignedInt i = 0; i < propObjectNames.size(); ++i)
         if (propObjectNames[i] == oldName)
            propObjectNames[i] = newName;
      retval = true;
   }

   return (Propagator::RenameRefObject(type, oldName, newName) || retval);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Passes reference objects in to the propagator
 *
 * @param obj The object
 * @param type The object's type
 * @param name The object's name
 *
 * @return true if the object was stored, otherwise false
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetRefObject(GmatBase *obj,
      const Gmat::ObjectType type, const std::string &name)
{
   #ifdef DEBUG_REF_OBJ
         MessageInterface::ShowMessage
               ("EphemerisPropagator::SetRefObject() <%s> entered, obj=<%p><%s>"
                "<%s>, type=%d, name='%s'\n", GetName().c_str(), obj,
                (obj ? obj->GetTypeName().c_str() : "NULL"),
                (obj ? obj->GetName().c_str() : "NULL"), type, name.c_str());
   #endif

   bool retval = false;

   if (obj->IsOfType(Gmat::SPACEOBJECT))
   {
      for (UnsignedInt i = 0; i < propObjectNames.size(); ++i)
         if (propObjectNames[i] == name)
         {
            propObjects[i] = obj;
            retval = true;
         }
      if (retval == false)
      {
         propObjectNames.push_back(name);
         propObjects.push_back(obj);
      }
      return true;
   }

   return (Propagator::SetRefObject(obj, type, name) || retval);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Passes reference objects in to the propagator for use in an object array
 *
 * @param obj The object
 * @param type The object's type
 * @param name The object's name
 * @param index Index of the object in the array
 *
 * @return true if the object was stored, otherwise false
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetRefObject(GmatBase *obj,
      const Gmat::ObjectType type, const std::string &name, const Integer index)
{
   bool retval = false;

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting object named %s at index %d\n",
            name.c_str(), index);
   #endif

   if (obj->IsOfType(Gmat::SPACEOBJECT))
   {
      if (propObjectNames[index] == name)
      {
         propObjects[index] = obj;
         retval = true;
      }
   }

   return (Propagator::SetRefObject(obj, type, name, index) || retval);
}


//------------------------------------------------------------------------------
// bool UsesODEModel()
//------------------------------------------------------------------------------
/**
 * Used to tell the PropSetup if an ODE model is needed for the propagator
 *
 * @return true if an ODEModel is required, false if not
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::UsesODEModel()
{
   return false;
}


//------------------------------------------------------------------------------
// void SetPropStateManager(PropagationStateManager *sm)
//------------------------------------------------------------------------------
/**
 * Sets the PSM for ephemeris based propagators
 *
 * @param sm The propagation state manager
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetPropStateManager(PropagationStateManager *sm)
{
   psm = sm;
}


//------------------------------------------------------------------------------
// bool EphemerisPropagator::Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the EphemeridPropagator for use during a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::Initialize()
{
   bool retval = false;

   if (Propagator::Initialize())
   {
      Real oldDim = dimension;
      dimension = 6 * propObjects.size();
      if (dimension > 0)
      {
         if (state != NULL)
            delete [] state;
         state = new Real[dimension];

         if ((j2kState != NULL) && (oldDim != dimension))
         {
            delete [] j2kState;
            j2kState = NULL;
         }

         if (j2kState == NULL)
            j2kState = new Real[dimension];
      }

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("State epoch %s from spacecraft\n",
               ((startEpochSource == FROM_SPACECRAFT) ? "is" : "is not"));
         MessageInterface::ShowMessage("State epoch %s from ephemeris\n",
               ((startEpochSource == FROM_EPHEM) ? "is" : "is not"));
      #endif
      switch (startEpochSource)
      {
         case FROM_SPACECRAFT:
            if (propObjects.size() > 0)
            {
               // Spacecraft update epochs during the run, so we have to update
               // timeFromEpoch for this case
               initialEpoch = ((SpaceObject*)(propObjects[0]))->GetEpoch();
               timeFromEpoch = 0.0;
               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("From spacecraft epoch = %lf, "
                        "elapsed time = %lf\n", initialEpoch, timeFromEpoch);
               #endif
            }
            break;

         case FROM_EPHEM:
            if (((SpaceObject*)(propObjects[0]))->HasEphemPropagated())
            {
               initialEpoch = ((SpaceObject*)(propObjects[0]))->GetEpoch();
               timeFromEpoch = 0.0;
            }
            else
               if (ephemStart > 0)
               {
                  initialEpoch = ephemStart;
               }
            break;

         case FROM_SCRIPT:
         default:
            if (((SpaceObject*)(propObjects[0]))->HasEphemPropagated())
            {
               initialEpoch = ((SpaceObject*)(propObjects[0]))->GetEpoch();
               timeFromEpoch = 0.0;
            }
            else
               initialEpoch = ConvertToRealEpoch(startEpoch, epochFormat);
            break;
      }

      if (currentEpoch == -987654321.0)
      {
         currentEpoch = initialEpoch;
      }

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Initial epoch set to %.12lf\n",
               initialEpoch);
         MessageInterface::ShowMessage("Current epoch set to %.12lf\n",
               currentEpoch);
      #endif

      // Set direction for the default step.  Step(Real) overrides the direction
      ephemStep = fabs(ephemStep) * stepDirection;

      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Advances the state vector by timestep dt
 *
 * @param dt The time step, in seconds
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::Step(Real dt)
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Stepping by %.12lf\n", dt);
   #endif

   bool retval = false;

   if (isInitialized)
   {
      Real tempStep = ephemStep;
      ephemStep = dt;
      retval = Step();
      if (retval)
         ((SpaceObject*)(propObjects[0]))->HasEphemPropagated(true);
      ephemStep = tempStep;
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer GetDimension()
//------------------------------------------------------------------------------
/**
 * Retrieves the size of the state vector that gets propagated
 *
 * @return The vector size
 */
//------------------------------------------------------------------------------
Integer EphemerisPropagator::GetDimension()
{
   if (dimension == 0)
      dimension = 6 * propObjects.size();
   return dimension;
}


//------------------------------------------------------------------------------
// Real* GetState()
//------------------------------------------------------------------------------
/**
 * Retrieves the Real state that gets propagated
 *
 * @return The state
 */
//------------------------------------------------------------------------------
Real* EphemerisPropagator::GetState()
{
   return state;
}


//------------------------------------------------------------------------------
// Real* GetJ2KState()
//------------------------------------------------------------------------------
/**
 * Retrieves the Real state that gets propagated in the J2000 j2kBody reference
 * frame
 *
 * @return The state
 */
//------------------------------------------------------------------------------
Real* EphemerisPropagator::GetJ2KState()
{
   return j2kState;
}


//------------------------------------------------------------------------------
// void UpdateSpaceObject(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Passes state data from propagator to the objects that are being propagated
 *
 * @param newEpoch The epoch of the state; use -1.0 to leave the epoch unchanged
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::UpdateSpaceObject(Real newEpoch)
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(
            "EphemerisPropagator::UpdateSpaceObject(%.12lf) called\n", newEpoch);
   #endif

   if (psm)
   {
      Integer stateSize;
      Integer vectorSize;
      GmatState *newState;
      ReturnFromOrigin(newEpoch);

      newState = psm->GetState();
      stateSize = newState->GetSize();
      vectorSize = stateSize * sizeof(Real);

      currentEpoch = initialEpoch + timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;

      // Update the epoch if it was passed in
      if (newEpoch != -1.0)
      {
         currentEpoch = newEpoch;
         timeFromEpoch = (currentEpoch-initialEpoch) * GmatTimeConstants::SECS_PER_DAY;
      }
      UpdateState();

      memcpy(newState->GetState(), j2kState, vectorSize);
      newState->SetEpoch(currentEpoch);
      psm->MapVectorToObjects();

//      // Formation code: Not part of initial release
//      // Update elements for each Formation
//      for (UnsignedInt i = 0; i < stateObjects.size(); ++i)
//         if (stateObjects[i]->IsOfType(Gmat::FORMATION))
//            ((Formation*)stateObjects[i])->UpdateElements();

      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage
               ("EphemerisPropagator::UpdateSpaceObject() on \"%s\" "
                "currentEpoch = %f, passed in epoch = %f\n", GetName().c_str(),
                currentEpoch, newEpoch);
      #endif
   }
}

//------------------------------------------------------------------------------
// void UpdateFromSpaceObject()
//------------------------------------------------------------------------------
/**
 * Fills the state vector with data from the objects that are being propagated
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::UpdateFromSpaceObject()
{
//   // Formation code: Not part of initial release
//   // Update elements for each Formation
//   for (UnsignedInt i = 0; i < stateObjects.size(); ++i)
//      if (stateObjects[i]->IsOfType(Gmat::FORMATION))
//         ((Formation*)stateObjects[i])->UpdateState();

   psm->MapObjectsToVector();
   GmatState *newState = psm->GetState();
   memcpy(state, newState->GetState(), newState->GetSize() * sizeof(Real));

    // Transform to the force model origin
    MoveToOrigin();
}


//------------------------------------------------------------------------------
// void RevertSpaceObject()
//------------------------------------------------------------------------------
/**
 * Restores SpaceObjects to the states stored in a buffer
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::RevertSpaceObject()
{
   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage
         ("ODEModel::RevertSpacecraft() prevElapsedTime=%f elapsedTime=%f\n",
          prevElapsedTime, elapsedTime);
   #endif

   timeFromEpoch = (previousState.GetEpoch() - initialEpoch) * GmatTimeConstants::SECS_PER_DAY;
   currentEpoch = initialEpoch + timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;
   UpdateState();

   MoveToOrigin();
}


//------------------------------------------------------------------------------
// void BufferState()
//------------------------------------------------------------------------------
/**
 * Buffers SpaceObjects for later restoration
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::BufferState()
{
   GmatState *stateToBuffer = psm->GetState();
   previousState = (*stateToBuffer);
}


//------------------------------------------------------------------------------
// Real GetTime()
//------------------------------------------------------------------------------
/**
 * Retrieves the time elapsed in the state from the propagator's base epoch
 *
 * @return The elapsed time
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetTime()
{
   return timeFromEpoch;
}


//------------------------------------------------------------------------------
// void SetTime(Real t)
//------------------------------------------------------------------------------
/**
 * Sets the time elapsed in the state from the propagator's base epoch
 *
 * @param t The elapsed time
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetTime(Real t)
{
   timeFromEpoch = t;
   currentEpoch = initialEpoch + timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;
   UpdateState();
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Real ConvertToRealEpoch(const std::string &theEpoch,
//                         const std::string &theFormat)
//------------------------------------------------------------------------------
/**
 * Converts an epoch string is a specified format into
 *
 * @param theEpoch The input epoch
 * @param theFormat The format of the input epoch
 *
 * @return
 */
//------------------------------------------------------------------------------
GmatEpoch EphemerisPropagator::ConvertToRealEpoch(const std::string &theEpoch,
                                   const std::string &theFormat)
{
   Real fromMjd = -999.999;
   Real retval = -999.999;
   std::string outStr;

   TimeConverterUtil::Convert(theFormat, fromMjd, theEpoch, "A1ModJulian",
         retval, outStr);

   if (retval == -999.999)
      throw PropagatorException("Error converting the time string \"" +
            theEpoch + "\"; please check the format for the input string.");
   return retval;
}


//------------------------------------------------------------------------------
// void SetEphemSpan(const GmatEpoch start, const GmatEpoch end)
//------------------------------------------------------------------------------
/**
 * Sets the ephem start and end epochs
 *
 * @param start The start epoch, in the A.1 modified Julian system
 * @param end The end epoch, in the A.1 modified Julian system
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetEphemSpan(const GmatEpoch start, const GmatEpoch end)
{
   if (end <= start)
      throw PropagatorException("The ephemeris propagator " + instanceName +
            " was passed an invalid span in the call to "
            "EphemerisPropagator::SetEphemSpan(const GmatEpoch start, "
            "const GmatEpoch end): start >= end");
   ephemStart = start;
   ephemEnd   = end;
}


//------------------------------------------------------------------------------
// void SetEphemSpan(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Sets the ephem start and end epochs for a given ephem
 *
 * @param whichOne The index of the ephem being processed
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetEphemSpan(Integer whichOne)
{
   throw PropagatorException("EphemerisPropagator::SetEphemSpan() is not "
         "implemented for the " + instanceName + "ephemeris propagator");
}


//------------------------------------------------------------------------------
// bool EphemerisPropagator::IsValidEpoch(GmatEpoch time)
//------------------------------------------------------------------------------
/**
 * Checks to see if an epoch is in the span covered by an ephemeris
 *
 * @param time The A.1 modified Julian epoch being checked
 *
 * @return true if the epoch is valid, false if not
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::IsValidEpoch(GmatEpoch time)
{
   bool retval = false;
   if ((time >= ephemStart) && (time <= ephemEnd))
      retval = true;
   return retval;
}


//------------------------------------------------------------------------------
// void MoveToOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Provides the interface that Propagators that do not use ODEModels use to
 * translate from the J2000 body to the propagator's central body
 *
 * @param newEpoch The epoch of the state that is translated
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::MoveToOrigin(Real newEpoch)
{
   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage("ODEModel::MoveToOrigin entered\n");
   #endif

   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage(
            "SatCount = %d, dimension = %d, stateSize = %d\n",cartObjCount,
            dimension, stateSize);
      MessageInterface::ShowMessage(
            "StatePointers: rawState = %p, modelState = %p\n", rawState,
            modelState);
      MessageInterface::ShowMessage(
          "ODEModel::MoveToOrigin()\n   Input state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", rawState[i]);
      MessageInterface::ShowMessage("]\n   model state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", modelState[i]);
      MessageInterface::ShowMessage("]\n\n");
   #endif

   memcpy(state, j2kState, dimension*sizeof(Real));

   if (centralBody != j2kBodyName)
   {
      Rvector6 cbState, mj2kState, delta;
      Real now = ((newEpoch < 0.0) ? currentEpoch : newEpoch);
      cbState = propOrigin->GetMJ2000State(now);
      mj2kState = j2kBody->GetState(now);

      delta = cbState - mj2kState;

      /**
       *  @todo When multiple spacecraft are supported, these items will need
       *  to be set to match the PSM's state vector
       */
      Integer cartObjCount = 1;
      Integer cartStateStart = 0;

      for (Integer i = 0; i < cartObjCount; ++i)
      {
         Integer i6 = cartStateStart + i * 6;
         for (int j = 0; j < 6; ++j)
            state[i6+j] -= delta[j];

         #ifdef DEBUG_REORIGIN
            MessageInterface::ShowMessage(
                "ODEModel::MoveToOrigin()\n"
                "   Input state: [%lf %lf %lf %lf %lf %lf]\n"
                "   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
                "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
                "   delta:       [%lf %lf %lf %lf %lf %lf]\n"
                "   model state: [%lf %lf %lf %lf %lf %lf]\n\n",
                rawState[i6], rawState[i6+1], rawState[i6+2], rawState[i6+3],
                rawState[i6+4], rawState[i6+5],
                mj2kState[0], mj2kState[1], mj2kState[2], mj2kState[3],
                mj2kState[4], mj2kState[5],
                cbState[0], cbState[1], cbState[2], cbState[3], cbState[4],
                cbState[5],
                delta[0], delta[1], delta[2], delta[3], delta[4], delta[5],
                modelState[i6], modelState[i6+1], modelState[i6+2],
                modelState[i6+3], modelState[i6+4], modelState[i6+5]);
         #endif
      }
   }

   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage(
          "   Move Complete\n   Input state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", rawState[i]);
      MessageInterface::ShowMessage("]\n   model state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", modelState[i]);
      MessageInterface::ShowMessage("]\n\n");

      MessageInterface::ShowMessage("ODEModel::MoveToOrigin Finished\n");
   #endif
}


//------------------------------------------------------------------------------
// void ReturnFromOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Provides the interface that Propagators that do not use ODEModels use to
 * translate from the propagator's central body to the J2000 body
 *
 * @param newEpoch The epoch of the state that is translated
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::ReturnFromOrigin(Real newEpoch)
{
   if ((j2kBody == NULL) || (propOrigin == NULL))
   {
      MessageInterface::ShowMessage("Cannot reorigin; j2kBody = %p, "
            "propOrigin = %p\n", j2kBody, propOrigin);
      return;
   }
   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage("ODEModel::ReturnFromOrigin entered\n");
   #endif

   memcpy(j2kState, state, dimension*sizeof(Real));
   if (centralBody != j2kBodyName)
   {
      Rvector6 cbState, jkState, delta;
      Real now = ((newEpoch < 0.0) ? currentEpoch : newEpoch);
      cbState = propOrigin->GetMJ2000State(now);
      jkState = j2kBody->GetState(now);

      delta = jkState - cbState;

      /**
       *  @todo When multiple spacecraft are supported, these items will need
       *  to be set to match the PSM's state vector
       */
      Integer cartObjCount = 1;
      Integer cartStateStart = 0;

      for (Integer i = 0; i < cartObjCount; ++i)
      {
         Integer i6 = cartStateStart + i * 6;
         for (int j = 0; j < 6; ++j)
            j2kState[i6+j] -= delta[j];
            #ifdef DEBUG_REORIGIN
               MessageInterface::ShowMessage(
                   "ODEModel::ReturnFromOrigin()\n   Input (model) state: [%lf %lf %lf %lf %lf"
                   " %lf]\n   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
                   "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
                   "   delta:       [%lf %lf %lf %lf %lf %lf]\n"
                   "   raw state: [%lf %lf %lf %lf %lf %lf]\n\n",
                   modelState[0], modelState[1], modelState[2], modelState[3], modelState[4],
                   modelState[5],
                   j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4],
                   j2kState[5],
                   cbState[0], cbState[1], cbState[2], cbState[3], cbState[4],
                   cbState[5],
                   delta[0], delta[1], delta[2], delta[3], delta[4], delta[5],
                   rawState[0], rawState[1], rawState[2], rawState[3],
                   rawState[4], rawState[5]);
         #endif
      }
   }
}
