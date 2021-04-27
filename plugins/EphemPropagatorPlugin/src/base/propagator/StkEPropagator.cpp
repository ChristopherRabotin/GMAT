//$Id$
//------------------------------------------------------------------------------
//                             StkEPropagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
 * Implementation for the StkEPropagator class
 */
//------------------------------------------------------------------------------


#include "StkEPropagator.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "PropagatorException.hpp"

#include <sstream>                     // for stringstream

//#define DEBUG_INITIALIZATION
//#define DEBUG_PROPAGATION
//#define DEBUG_INTERPOLATION

#define PAUSE_AT_BOUNDS


//---------------------------------
// static data
//---------------------------------

/// StkEPropagator parameter labels
const std::string StkEPropagator::PARAMETER_TEXT[
                 StkEPropagatorParamCount - EphemerisPropagatorParamCount] =
{
      "EphemFile"                  //EPHEMERISFILENAME
};

/// StkEPropagator parameter types
const Gmat::ParameterType StkEPropagator::PARAMETER_TYPE[
                 StkEPropagatorParamCount - EphemerisPropagatorParamCount] =
{
      Gmat::FILENAME_TYPE          //EPHEMERISFILENAME
};


//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// StkEPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the object that gets constructed
 */
//------------------------------------------------------------------------------
StkEPropagator::StkEPropagator(const std::string &name) :
   EphemerisPropagator        ("STK", name),
   ephemName                  (""),
   fileDataLoaded             (false),
   ephemRecords               (NULL),
   stateIndex                 (-1),
   timeFromEphemStart         (-1.0),
   lastEpoch                  (-1.0),
   lastEpochGT                (-1.0)
{
   // GmatBase data
  objectTypeNames.push_back("StkEPropagator");
  parameterCount        = StkEPropagatorParamCount;
  theEphem              = &ephem;
}


//------------------------------------------------------------------------------
// ~StkEPropagator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
StkEPropagator::~StkEPropagator()
{
}


//------------------------------------------------------------------------------
// StkEPropagator(const StkEPropagator & prop)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param prop The object that is copied into this new one
 */
//------------------------------------------------------------------------------
StkEPropagator::StkEPropagator(const StkEPropagator & prop) :
   EphemerisPropagator        (prop),
   ephemName                  (prop.ephemName),
   fileDataLoaded             (false),
   ephemRecords               (NULL),
   stateIndex                 (-1),
   timeFromEphemStart         (-1.0),
   lastEpoch                  (-1.0),
   lastEpochGT                (-1.0)
{
   theEphem              = &ephem;
}


//------------------------------------------------------------------------------
// StkEPropagator & StkEPropagator::operator =(const StkEPropagator & prop)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param prop The object that is provides data for into this one
 *
 * @return This propagator, configured to match prop.
 */
//------------------------------------------------------------------------------
StkEPropagator & StkEPropagator::operator=(const StkEPropagator & prop)
{
   if (this != &prop)
   {
      EphemerisPropagator::operator=(prop);

      ephemName = prop.ephemName;
      fileDataLoaded = false;
      ephemRecords = NULL;
      stateIndex = -1;

      lastEpoch = currentEpoch = prop.currentEpoch;
      lastEpochGT = currentEpochGT = prop.currentEpochGT;

      if (hasPrecisionTime)
      {
         if (lastEpochGT != -1.0)
            timeFromEphemStart = (lastEpochGT - GmatTime(ephemStart)).GetTimeInSec();
         else
            timeFromEphemStart = -1.0;
      }
      else
      {
         if (lastEpoch != -1.0)
            timeFromEphemStart = (lastEpoch - ephemStart) *
            GmatTimeConstants::SECS_PER_DAY;
         else
            timeFromEphemStart = -1.0;
      }
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
GmatBase* StkEPropagator::Clone() const
{
   return new StkEPropagator(*this);
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
std::string StkEPropagator::GetParameterText(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < StkEPropagatorParamCount)
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
Integer StkEPropagator::GetParameterID(const std::string &str) const
{
   for (Integer i = EphemerisPropagatorParamCount;
         i < StkEPropagatorParamCount; ++i)
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
Gmat::ParameterType StkEPropagator::GetParameterType(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < StkEPropagatorParamCount)
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
std::string StkEPropagator::GetParameterTypeString(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < StkEPropagatorParamCount)
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
std::string StkEPropagator::GetParameterUnit(const Integer id) const
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
bool StkEPropagator::IsParameterReadOnly(const Integer id) const
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
bool StkEPropagator::IsParameterReadOnly(const std::string &label) const
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
std::string StkEPropagator::GetStringParameter(const Integer id) const
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
bool StkEPropagator::SetStringParameter(const Integer id,
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
std::string StkEPropagator::GetStringParameter(const Integer id,
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
bool StkEPropagator::SetStringParameter(const Integer id,
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
const StringArray& StkEPropagator::GetStringArrayParameter(
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
const StringArray& StkEPropagator::GetStringArrayParameter(const Integer id,
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
std::string StkEPropagator::GetStringParameter(const std::string &label) const
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
bool StkEPropagator::SetStringParameter(const std::string &label,
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
std::string StkEPropagator::GetStringParameter(const std::string &label,
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
bool StkEPropagator::SetStringParameter(const std::string &label,
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
const StringArray& StkEPropagator::GetStringArrayParameter(
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
const StringArray& StkEPropagator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the StkEPropagator for use in a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool StkEPropagator::Initialize()
{
   Integer logOption = 0;
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("StkEPropagator::Initialize() entered\n");
      logOption = 1;
   #endif
   bool retval = false;

   if (EphemerisPropagator::Initialize())
   {
      stepTaken = 0.0;

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("   After base class initialization, "
               "initialEpoch = %.12lf\n", initialEpoch);
      #endif

      FileManager *fm = FileManager::Instance();
      std::string fullPath;

      if (propObjects.size() != 1)
         throw PropagatorException("STK propagators require exactly one "
               "SpaceObject.");

      // The PSM isn't set until PrepareToPropagate fires.  The following is
      // also last minute setup, so only do it if the PSM has been set
      if ((psm != NULL) && !fileDataLoaded)
      {
         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
            if (propObjects[i]->IsOfType(Gmat::SPACECRAFT))
            {
               ephemName = propObjects[i]->GetStringParameter(
                     "EphemerisName");

               currentEpoch = ((Spacecraft*)propObjects[i])->GetEpoch();
               currentEpochGT = ((Spacecraft*)propObjects[i])->GetEpochGT();

               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Spacecraft epoch is %.12lf\n",
                        currentEpoch);
                #endif
            }
            else
               throw PropagatorException("STK ephemeris propagators only "
                     "work for Spacecraft.");

            if (ephemName == "")
               throw PropagatorException("The STK propagator requires a "
                     "valid ephemeris file name");

            fullPath = fm->FindPath(ephemName, "VEHICLE_EPHEM_PATH", true, false, true);
            if (fullPath == "")
               throw PropagatorException("The STK ephemeris file " +
                     ephemName + " does not exist");

            if (!ephem.OpenForRead(fullPath, "TimePosVel"))
               throw PropagatorException("The STK ephemeris file " +
                     ephemName + " failed to open");

            Integer dumpdata = 0;
            #ifdef DEBUG_EPHEM_READ
               dumpdata = 1;
            #endif
            ephem.ReadDataRecords(dumpdata);
            ephem.GetStartAndEndEpochs(ephemStart, ephemEnd, &ephemRecords);
            fileDataLoaded = true;

            // Setup central body
            centralBody = ephem.GetCentralBody();
            if (centralBody == "Moon")
               centralBody = "Luna";
            if (centralBody == "")     // STK .e spec: Use "vehicle CB, and Earth by default"
               centralBody = "Earth";

            propOrigin = solarSystem->GetBody(centralBody);

            ephem.CloseForRead();

            Rvector6 interpVal = ephem.InterpolatePoint(currentEpoch);
            std::memcpy(state, interpVal.GetDataVector(),
                  6*sizeof(Real));
            lastEpoch = currentEpoch;

            timeFromEphemStart = (lastEpoch - ephemStart) *
                  GmatTimeConstants::SECS_PER_DAY;

            UpdateSpaceObject(currentEpoch);
         }
      }
   }

   // @todo: This is likely the source of GMT-5959
   if (startEpochSource == FROM_SCRIPT)
      for (UnsignedInt i = 0; i < propObjects.size(); ++i)
      {
         propObjects[i]->SetRealParameter("A1Epoch", currentEpoch);
         if (hasPrecisionTime)
            propObjects[i]->SetGmatTimeParameter("A1Epoch", currentEpochGT);
         else
            propObjects[i]->SetGmatTimeParameter("A1Epoch", GmatTime(currentEpoch));
      }


   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("StkEPropagator::Initialize(), on exit, "
            "initialEpoch = %.12lf, current = %.12lf\n", initialEpoch,
            currentEpoch);
   #endif

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
//@todo: need GmatTime modification
bool StkEPropagator::Step()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("StkEPropagator::Step() entered for %p: "
            "initialEpoch = %.12lf; stepsize = %.12lf; "
            "timeFromEpoch = %.12lf\n", this, initialEpoch, ephemStep,
            timeFromEpoch);
   #endif

   bool retval = false;

   Rvector6  outState;

   #ifdef DEBUG_FINALSTEP
      if (currentEpoch == ephemEnd)
         MessageInterface::ShowMessage("Stepping off the end of the ephem\n");
   #endif

   if (lastEpoch != currentEpoch)
   {
      lastEpoch = currentEpoch;
      timeFromEphemStart = (lastEpoch - ephemStart) *
            GmatTimeConstants::SECS_PER_DAY;
   }

   timeFromEphemStart += ephemStep;
   timeFromEpoch += ephemStep;
   stepTaken = ephemStep;

   currentEpoch = ephemStart + timeFromEphemStart /
         GmatTimeConstants::SECS_PER_DAY;

   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("   ephemStart = %.12lf, timeFromStart = "
         "%lf sec => currentEpoch after step = %.12lf; lastEpoch = %.12lf; "
         "ephemEnd = %.12lf\n", ephemStart, timeFromEphemStart, currentEpoch,
         lastEpoch, ephemEnd);
   #endif

   #ifdef PAUSE_AT_BOUNDS
      // Code to step to ephem bound before stepping out of bounds
      if ((lastEpoch < ephemEnd) && (currentEpoch > ephemEnd))
         currentEpoch = ephemEnd;
      if ((lastEpoch > ephemStart) && (currentEpoch < ephemStart))
         currentEpoch = ephemStart;
      #ifdef DEBUG_PROPAGATION
         MessageInterface::ShowMessage("   --> Updated currentEpoch after step "
               "= %.12lf\n", currentEpoch);
      #endif
   #endif

   // Allow for slop in the last few bits
   bool flagOutOfDomain = false;
   if (currentEpoch < ephemStart)
   {
      if (ephemStart - currentEpoch < 1.0e-10)
         currentEpoch = ephemStart;
      else
         flagOutOfDomain = true;
   }
   else if (currentEpoch > ephemEnd)
   {
      if (currentEpoch - ephemEnd < 1.0e-10)
         currentEpoch = ephemEnd;
      else
         flagOutOfDomain = true;

      #ifdef DEBUG_FINALSTEP
         MessageInterface::ShowMessage("******** ==> Triggered step past end code\n");
      #endif
   }

   if (flagOutOfDomain)
   {
      std::stringstream errmsg;
      errmsg.precision(16);
      errmsg << "The STK Propagator "
             << instanceName
             << " is attempting to step outside of the span of the "
                "ephemeris data; halting.  ";
      errmsg << "The current STK ephemeris covers the A.1 modified "
                "Julian span ";
      errmsg << ephemStart << " to " << ephemEnd << " and the "
            "requested epoch is " << currentEpoch << ".";
      throw PropagatorException(errmsg.str());
   }

   #ifdef DEBUG_FINALSTEP
      MessageInterface::ShowMessage("Last epoch: %.12lf Stepping to %.12lf; step "
         "size %.12lf\n", lastEpoch, currentEpoch,
         (currentEpoch - lastEpoch) * 86400.0);
   #endif
   
   Rvector6 interpVal = ephem.InterpolatePoint(currentEpoch);
   lastEpoch = currentEpoch;
   std::memcpy(state, interpVal.GetDataVector(), 6*sizeof(Real));

   #ifdef DEBUG_FINALSTEP
      if (currentEpoch == ephemEnd)
         MessageInterface::ShowMessage("Stepped to the end of the ephem\n");
   #endif
   
   UpdateSpaceObject(currentEpoch);

   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("(Step for %p) State at epoch "
            "%.12lf is [", this, currentEpoch);
      for (Integer i = 0; i < dimension; ++i)
      {
         MessageInterface::ShowMessage("%.12lf", state[i]);
         if (i < 5)
            MessageInterface::ShowMessage("   ");
         else
            MessageInterface::ShowMessage("]\n");
      }
   #endif

   retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// bool RawStep()
//------------------------------------------------------------------------------
/**
 * Performs a propagation step without error control
 *
 * @note: RawStep is not used with the StkEPropagator
 *
 * @return false always
 */
//------------------------------------------------------------------------------
bool StkEPropagator::RawStep()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Real GetStepTaken()
//------------------------------------------------------------------------------
/**
 * Retrieves the size of the most recent StkEPropagator step
 *
 * @return The most recent step (0.0 if no step was taken with this instance).
 */
//------------------------------------------------------------------------------
Real StkEPropagator::GetStepTaken()
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
void StkEPropagator::UpdateState()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Updating state to epoch %.12lf\n",
            currentEpoch);
   #endif

   Rvector6 theState = ephem.InterpolatePoint(currentEpoch);
   std::memcpy(state, theState.GetDataVector(), 6*sizeof(Real));
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
void StkEPropagator::SetEphemSpan(Integer whichOne)
{
   ephem.GetStartAndEndEpochs(ephemStart, ephemEnd, &ephemRecords);
}

////------------------------------------------------------------------------------
//// void StkEPropagator::FindRecord(GmatEpoch forEpoch)
////------------------------------------------------------------------------------
///**
// * Sets up the indices into the ephem data for a input epoch
// *
// * On return, stateIndex points to the record of the data point matching or
// * earlier than forEpoch.
// *
// * @param forEpoch The epoch that is being set up
// */
////------------------------------------------------------------------------------
//void StkEPropagator::FindRecord(GmatEpoch forEpoch)
//{
//   GmatEpoch currentEpoch;
//
//   stateIndex = -1;
//   for (UnsignedInt i = 0; i < ephemRecords->size(); ++i)
//   {
//      currentEpoch = ephemStart + ephemRecords->at(i).timeFromEpoch /
//              GmatTimeConstants::SECS_PER_DAY;
//
//      if (currentEpoch > forEpoch)
//         break;
//
//      stateIndex = i;
//   }
//}
//
////------------------------------------------------------------------------------
//// void GetState(GmatEpoch forEpoch, Rvector6 &outstate)
////------------------------------------------------------------------------------
///**
// * Returns the state data at the specified epoch
// *
// * @param forEpoch The epoch for the data
// * @param outstate The state vector that receives the data
// */
////------------------------------------------------------------------------------
//void StkEPropagator::GetState(GmatEpoch forEpoch, Rvector6 &outstate)
//{
//   UpdateInterpolator(forEpoch);
//
//   Real theState[6];
//   if (interp->Interpolate(forEpoch, theState))
//      outstate.Set(theState);
//   else
//   {
//      std::stringstream date;
//      date.precision(16);
//      date << forEpoch;
//
//      throw PropagatorException("The propagator " + instanceName +
//               " failed to interpolate a valid state for " +
//               propObjects[0]->GetName() + " for epoch " + date.str());
//   }
//
//   #ifdef DEBUG_INTERPOLATION
//      MessageInterface::ShowMessage("Interpolated state: %.12lf  [%.15lf  "
//            "%.15lf  %.15lf  %.15lf  %.15lf  %.15lf]\n", forEpoch, theState[0],
//            theState[1], theState[2], theState[3], theState[4], theState[5]);
//   #endif
//}
