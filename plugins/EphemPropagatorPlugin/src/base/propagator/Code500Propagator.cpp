//$Id$
//------------------------------------------------------------------------------
//                             Code500Propagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Darrel J. Conway
// Created: Apr 08, 2016 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task 26
//
/**
 * Implementation for the Code500Propagator class
 */
//------------------------------------------------------------------------------


#include "Code500Propagator.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include <sstream>                 // for stringstream

#define DEBUG_INITIALIZATION
#define DEBUG_PROPAGATION
//#define TEST_TDB_ROUND_TRIP

//---------------------------------
// static data
//---------------------------------

/// Code500Propagator parameter labels
const std::string Code500Propagator::PARAMETER_TEXT[
                 Code500PropagatorParamCount - EphemerisPropagatorParamCount] =
{
      "EphemFile"                  //EPHEMERISFILENAME
};

/// Code500Propagator parameter types
const Gmat::ParameterType Code500Propagator::PARAMETER_TYPE[
                 Code500PropagatorParamCount - EphemerisPropagatorParamCount] =
{
      Gmat::FILENAME_TYPE        //EPHEMERISFILENAME
};


//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Code500Propagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the object that gets constructed
 */
//------------------------------------------------------------------------------
Code500Propagator::Code500Propagator(const std::string &name) :
   EphemerisPropagator        ("Code500", name),
   ephemName                  (""),
   satID                      (101.0),
   fileIsOpen                 (false)
{
   // GmatBase data
  objectTypeNames.push_back("Code500Propagator");
  parameterCount       = Code500PropagatorParamCount;
}


//------------------------------------------------------------------------------
// ~Code500Propagator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Code500Propagator::~Code500Propagator()
{
}


//------------------------------------------------------------------------------
// Code500Propagator(const Code500Propagator & prop)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param prop The object that is copied into this new one
 */
//------------------------------------------------------------------------------
Code500Propagator::Code500Propagator(const Code500Propagator & prop) :
   EphemerisPropagator        (prop),
   ephemName                  (prop.ephemName),
   satID                      (prop.satID),
   fileIsOpen                 (false)
{
}


//------------------------------------------------------------------------------
// Code500Propagator & Code500Propagator::operator =(const Code500Propagator & prop)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param prop The object that is provides data for into this one
 *
 * @return This propagator, configured to match prop.
 */
//------------------------------------------------------------------------------
Code500Propagator & Code500Propagator::operator =(const Code500Propagator & prop)
{
   if (this != &prop)
   {
      EphemerisPropagator::operator=(prop);

      ephemName = prop.ephemName;
      satID = prop.satID;
      if (fileIsOpen)
         ephem.CloseForRead();
      fileIsOpen = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Generates a new object that matches this one
 *
 * @return The new object
 */
//------------------------------------------------------------------------------
GmatBase* Code500Propagator::Clone() const
{
   return new Code500Propagator(*this);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script string for a parameter
 *
 * @param id The index of the parameter in the parameter tables
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetParameterText(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < Code500PropagatorParamCount)
      return PARAMETER_TEXT[id - EphemerisPropagatorParamCount];
   return EphemerisPropagator::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID of a parameter
 *
 * @param The script string for the parameter
 *
 * @return The parameter's ID
 */
//------------------------------------------------------------------------------
Integer Code500Propagator::GetParameterID(const std::string &str) const
{
   for (Integer i = EphemerisPropagatorParamCount;
         i < Code500PropagatorParamCount; ++i)
   {
       if (str == PARAMETER_TEXT[i - EphemerisPropagatorParamCount])
           return i;
   }

   return EphemerisPropagator::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Code500Propagator::GetParameterType(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < Code500PropagatorParamCount)
      return PARAMETER_TYPE[id - EphemerisPropagatorParamCount];
   return EphemerisPropagator::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string description of a parameter's type
 *
 * @param id The ID of the parameter
 *
 * @return The type of the parameter
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetParameterTypeString(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < Code500PropagatorParamCount)
      return EphemerisPropagator::PARAM_TYPE_STRING[GetParameterType(id)];
   return EphemerisPropagator::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * retrieves the dimensional units for a parameter
 *
 * @param id The ID of the parameter
 *
 * @return The unit label
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetParameterUnit(const Integer id) const
{
   return EphemerisPropagator::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Reports if a parameter should be hidden from the users
 *
 * @param id The ID of the parameter
 *
 * @return true if the parameter should be hidden, false if not
 */
//------------------------------------------------------------------------------
bool Code500Propagator::IsParameterReadOnly(const Integer id) const
{
   if (id == EPHEMERISFILENAME)
      return true;
   return EphemerisPropagator::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Reports if a parameter should be hidden from the users
 *
 * @param label The scripted string of the parameter
 *
 * @return true if the paameter should be hidden, false if not
 */
//------------------------------------------------------------------------------
bool Code500Propagator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetStringParameter(const Integer id) const
{
   return EphemerisPropagator::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter
 *
 * @param id The ID of the parameter
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::SetStringParameter(const Integer id,
      const std::string &value)
{

   if (id == EPHEMERISFILENAME)
   {

      return true;         // Idempotent, so return true
   }

   bool retval = EphemerisPropagator::SetStringParameter(id, value);

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from an array
 *
 * @param id The ID of the parameter
 * @param index The array index
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == EPHEMERISFILENAME)
   {
      return "";
   }

   return EphemerisPropagator::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array
 *
 * @param id The ID of the parameter
 * @param value The new value
 * @param index The index of the parameter in the array
 *
 * @return True on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   if (id == EPHEMERISFILENAME)
   {
      return false;
   }

   return EphemerisPropagator::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param id The ID of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Code500Propagator::GetStringArrayParameter(
      const Integer id) const
{
   return EphemerisPropagator::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter from an array of StringArrays
 *
 * @param id The ID of the parameter
 * @param index The index of the StringArray
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Code500Propagator::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return EphemerisPropagator::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param label The script label of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter
 *
 * @param label The script label of the parameter
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from an array
 *
 * @param label The script label of the parameter
 * @param index The array index
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array
 *
 * @param label The script label of the parameter
 * @param value The new value
 * @param index The index of the parameter in the array
 *
 * @return True on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param label The script label of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Code500Propagator::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter from an array of StringArrays
 *
 * @param label The script label of the parameter
 * @param index The index of the StringArray
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Code500Propagator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the Code500Propagator for use in a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::Initialize()
{
   Integer logOption = 0;
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Code500Propagator::Initialize() entered\n");
      logOption = 1;
   #endif
   bool retval = false;

   if (EphemerisPropagator::Initialize())
   {
      stepTaken = 0.0;

      FileManager *fm = FileManager::Instance();
      std::string fullPath;

      if (propObjects.size() != 1)
         throw PropagatorException("Code 500 propagators require exactly one "
               "SpaceObject.");

      // The PSM isn't set until PrepareToPropagate fires.  The following is
      // also last minute setup, so only do it if the PSM has been set
      if (psm != NULL)
      {
         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
//            Real id = propObjects[i]->GetRealParameter("SatelliteID");
//            satID = id;

            if (propObjects[i]->IsOfType(Gmat::SPACECRAFT))
               ephemName = propObjects[i]->GetStringParameter(
                     "EphemerisName");
            else
               throw PropagatorException("Code 500 ephemeris propagators only "
                     "work for Spacecraft.");

            if (ephemName == "")
               throw PropagatorException("The Code-500 propagator requires a "
                     "valid ephemeris file name");

            fullPath = fm->FindPath(ephemName, "VEHICLE_EPHEM_PATH", true, false, true);
            if (fullPath == "")
               throw PropagatorException("The Code 500 ephemeris file " +
                     ephemName + " does not exist");

            if (!ephem.OpenForRead(fullPath))
               throw PropagatorException("The Code 500 ephemeris file " +
                     ephemName + " failed to open");

            ephem.ReadHeader1(logOption);
            ephem.ReadHeader2(logOption);
            fileIsOpen = true;
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool Step()
//------------------------------------------------------------------------------
/**
 * Advances the state vector by the ephem step
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::Step()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Code500Propagator::Step() entered: "
            "initialEpoch = %.12lf; stepsize = %.12lf; "
            "timeFromEpoch = %.12lf\n", initialEpoch, ephemStep, timeFromEpoch);
   #endif

   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool RawStep()
//------------------------------------------------------------------------------
/**
 * Performs a propagation step without error control
 *
 * @note: RawStep is not used with the Code500Propagator
 *
 * @return false always
 */
//------------------------------------------------------------------------------
bool Code500Propagator::RawStep()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Real GetStepTaken()
//------------------------------------------------------------------------------
/**
 * Retrieves the size of the most recent Code500Propagator step
 *
 * @return The most recent step (0.0 if no step was taken with this instance).
 */
//------------------------------------------------------------------------------
Real Code500Propagator::GetStepTaken()
{
   return stepTaken;
}


//------------------------------------------------------------------------------
// void UpdateState()
//------------------------------------------------------------------------------
/**
 * Updates the propagation state vector with data from the
 * PropagationStateManager
 */
//------------------------------------------------------------------------------
void Code500Propagator::UpdateState()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Updating state to epoch %.12lf\n",
            currentEpoch);
   #endif

//   if (skr)
//   {
//      try
//      {
//         Rvector6  outState;
//
//         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
//         {
//            std::string scName = propObjectNames[i];
//            Integer id = naifIds[i];
//
//            // Allow for slop in the last few bits
//            if ((currentEpoch < ephemStart - 1e-10) ||
//                (currentEpoch > ephemEnd + 1e-10))
//            {
//               std::stringstream errmsg;
//               errmsg.precision(16);
//               errmsg << "The Code500Propagator "
//                      << instanceName
//                      << " is attempting to access state data outside of the "
//                         "span of the ephemeris data; halting.  ";
//               errmsg << "The current Code 500 ephemeris covers the A.1 "
//                      << "modified Julian span "
//                      << ephemStart << " to " << ephemEnd << " and the "
//                         "requested epoch is " << currentEpoch << ".";
//               throw PropagatorException(errmsg.str());
//            }
//
////            outState = skr->GetTargetState(scName, id, currentEpoch,
////                  spkCentralBody, spkCentralBodyNaifId);
//
//            std::memcpy(state, outState.GetDataVector(),
//                  dimension*sizeof(Real));
//
//            #ifdef DEBUG_PROPAGATION
//               MessageInterface::ShowMessage("(UpdateState for %p) State at "
//                     "epoch %.12lf is [", this, currentEpoch);
//               for (Integer i = 0; i < dimension; ++i)
//               {
//                  MessageInterface::ShowMessage("%.12lf", state[i]);
//                  if (i < 5)
//                     MessageInterface::ShowMessage("   ");
//                  else
//                     MessageInterface::ShowMessage("]\n");
//               }
//            #endif
//         }
//      }
//      catch (BaseException &e)
//      {
//         MessageInterface::ShowMessage(e.GetFullMessage());
//         throw;
//      }
//   }
}


//------------------------------------------------------------------------------
// void SetEphemSpan(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Determines the start and end epoch for the SPICE ephemerides associated with
 * the propagated spacecraft
 *
 * @param whichOne Not currrently used.
 */
//------------------------------------------------------------------------------
void Code500Propagator::SetEphemSpan(Integer whichOne)
{
}
