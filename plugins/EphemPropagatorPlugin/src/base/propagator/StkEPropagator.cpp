//$Id$
//------------------------------------------------------------------------------
//                             StkEPropagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
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
#include "NotAKnotInterpolator.hpp"    // Only one supported for now
#include "PropagatorException.hpp"

#include <sstream>                     // for stringstream

//#define DEBUG_INITIALIZATION
//#define DEBUG_PROPAGATION
//#define DEBUG_INTERPOLATION


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
   interp                     (NULL),
   fileDataLoaded             (false),
   ephemRecords               (NULL),
   stateIndex                 (-1),
   timeFromEphemStart         (-1.0),
   lastEpoch                  (-1.0)
{
   // GmatBase data
  objectTypeNames.push_back("StkEPropagator");
  parameterCount       = StkEPropagatorParamCount;
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
   if (interp != NULL)
      delete interp;
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
   interp                     (NULL),
   fileDataLoaded             (false),
   ephemRecords               (NULL),
   stateIndex                 (-1),
   timeFromEphemStart         (-1.0),
   lastEpoch                  (-1.0)
{
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
      if (interp != NULL)
      {
         delete interp;
         interp = NULL;
      }
      fileDataLoaded = false;
      ephemRecords = NULL;
      stateIndex = -1;
      lastEpoch = currentEpoch = prop.currentEpoch;
      if (lastEpoch != -1.0)
         timeFromEphemStart = (lastEpoch - ephemStart) *
               GmatTimeConstants::SECS_PER_DAY;
      else
         timeFromEphemStart = -1.0;
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

               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Spacecraft epoch is %.12lf\n",
                        currentEpoch);
                #endif
            }
            else
               throw PropagatorException("STK ephemeris ephemeris propagators only "
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

            // Build the interpolator.  For now, use not-a-knot splines
            if (interp != NULL)
               delete interp;
            interp = new NotAKnotInterpolator("STKEphemNotAKnot", 6);
            ephem.CloseForRead();
         }
      }
   }

   // @todo: This is likely the source of GMT-5959
   if (startEpochSource == FROM_SCRIPT)
      for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         propObjects[i]->SetRealParameter("A1Epoch", currentEpoch);


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
bool StkEPropagator::Step()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("StkEPropagator::Step() entered for %p: "
            "initialEpoch = %.12lf; stepsize = %.12lf; "
            "timeFromEpoch = %.12lf\n", this, initialEpoch, ephemStep,
            timeFromEpoch);
   #endif

   bool retval = false;

   // Might need to do this:
   // currentEpoch = ((Spacecraft*)propObjects[i])->GetEpoch();
   if (stateIndex == -1)
   {
      // Initialize the pointers into the ephem data
      FindRecord(currentEpoch);
   }

   if (stateIndex < 0)
      throw PropagatorException("Unable to propagate " + instanceName +
            ": is the epoch outside of the span of the ephemeris file?");

   Rvector6  outState;

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
	   "ephemEnd = %.12lf\n",
            ephemStart, timeFromEphemStart, currentEpoch, lastEpoch, ephemEnd);
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

   GetState(currentEpoch, outState);
   lastEpoch = currentEpoch;

   std::memcpy(state, outState.GetDataVector(),
         dimension*sizeof(Real));

   //MoveToOrigin(currentEpoch);

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

//   Rvector6 theState;
//   GetState(currentEpoch, theState);
//   std::memcpy(state, theState.GetDataVector(),
//         dimension*sizeof(Real));
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

//------------------------------------------------------------------------------
// void StkEPropagator::FindRecord(GmatEpoch forEpoch)
//------------------------------------------------------------------------------
/**
 * Sets up the indices into the ephem data for a input epoch
 *
 * On return, stateIndex points to the record of the data point matching or
 * earlier than forEpoch.
 *
 * @param forEpoch The epoch that is being set up
 */
//------------------------------------------------------------------------------
void StkEPropagator::FindRecord(GmatEpoch forEpoch)
{
   GmatEpoch currentEpoch;

   stateIndex = -1;
   for (UnsignedInt i = 0; i < ephemRecords->size(); ++i)
   {
      currentEpoch = ephemStart + ephemRecords->at(i).timeFromEpoch /
              GmatTimeConstants::SECS_PER_DAY;

      if (currentEpoch > forEpoch)
         break;

      stateIndex = i;
   }
}

//------------------------------------------------------------------------------
// void GetState(GmatEpoch forEpoch, Rvector6 &outstate)
//------------------------------------------------------------------------------
/**
 * Returns the state data at the specified epoch
 *
 * @param forEpoch The epoch for the data
 * @param outstate The state vector that receives the data
 */
//------------------------------------------------------------------------------
void StkEPropagator::GetState(GmatEpoch forEpoch, Rvector6 &outstate)
{
   UpdateInterpolator(forEpoch);

   Real theState[6];
   if (interp->Interpolate(forEpoch, theState))
      outstate.Set(theState);
   else
   {
      std::stringstream date;
	  date.precision(16);
	  date << forEpoch;

      throw PropagatorException("The propagator " + instanceName +
               " failed to interpolate a valid state for " +
               propObjects[0]->GetName() + " for epoch " + date.str());
   }

   #ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("Interpolated state: %.12lf  [%.15lf  "
            "%.15lf  %.15lf  %.15lf  %.15lf  %.15lf]\n", forEpoch, theState[0],
            theState[1], theState[2], theState[3], theState[4], theState[5]);
   #endif
}

//------------------------------------------------------------------------------
// void UpdateInterpolator(forEpoch)
//------------------------------------------------------------------------------
/**
 * Method that updates buffer data in the interpolator as propagation proceeds
 *
 * This method passes ephem data to the interpolator and resets the time span
 * data to track the data loaded.  The "sweet" region used for interpolation
 * moves as propagation progresses, and is generally chosen, when possible, to
 * keep the points used for interpolation centered on the interpolation epoch.
 * Thus for the cubic spline interpolator used in this implementation, the
 * interpolator data is set so that the interpolation epoch (forEpoch) falls
 * between the second and third ephemeris points when possible, minimizing the
 * likelihood that the interpolation will ring.
 *
 * @param forEpoch The epoch that needs to be covered in the preferred region of
 *                 the interpolator data
 */
//------------------------------------------------------------------------------
void StkEPropagator::UpdateInterpolator(const GmatEpoch &forEpoch)
{
   GmatEpoch useEpoch = forEpoch;

   if ((forEpoch < ephemStart) || (forEpoch > ephemEnd))
   {
      if (ephemStart - forEpoch < 1.0e-10)
         useEpoch = ephemStart;
      if (forEpoch - ephemEnd < 1.0e-10)
         useEpoch = ephemEnd;
   }

   FindRecord(useEpoch);

   if (stateIndex == -1)
      throw PropagatorException("Requested epoch is outside of the span "
            "covered by the ephemeris file " + ephemName);

   Integer startIndex = stateIndex - 1;
   Integer endIndex   = stateIndex + 3;

   while (startIndex < 0)
   {
      ++startIndex;
      ++endIndex;
   }

   while (endIndex > ephemRecords->size() - 1)
   {
      --startIndex;
      --endIndex;
   }

   if (startIndex < 0)
      throw PropagatorException("Insufficient ephemeris data for propagation");

   Real epoch;
   Real state[6];
   Real prevState[6];
   Integer dupIndex = -1;
   Real dupEpoch;

   // Brute force for now: refill the interpolator
   interp->Clear();

   #ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("Pairs used for epoch %.12lf:\n", forEpoch);
   #endif

   for (UnsignedInt i = startIndex; i <= endIndex; ++i)
   {
      epoch = ephemStart + ephemRecords->at(i).timeFromEpoch /
            GmatTimeConstants::SECS_PER_DAY;

      for (UnsignedInt j = 0; j < 3; ++j)
      {
         state[j]   = ephemRecords->at(i).theState[j];
         state[j+3] = ephemRecords->at(i).theState[j+3];
	  }

//	  if (prevState[0] == state[0] && prevState[1] == state[1] && prevState[2] == state[2] && prevState[3] == state[3]) {
//		  epoch = ephemStart + ephemRecords->at(i+1).timeFromEpoch /
//			  GmatTimeConstants::SECS_PER_DAY;
//
//		  for (UnsignedInt j = 0; j < 3; ++j)
//		  {
//			  state[j]   = ephemRecords->at(i+1).theState[j];
//			  state[j+3] = ephemRecords->at(i+1).theState[j+3];
//		  }
//	  }

	  if (prevState[0] == state[0]) {
		  dupIndex = i;
		  dupEpoch = epoch;
		  break;
	  }

	  prevState[0] = state[0];
	  prevState[1] = state[1];
	  prevState[2] = state[2];
	  prevState[3] = state[3];
	  prevState[4] = state[4];
	  prevState[5] = state[5];
	  prevState[6] = state[6];

      interp->AddPoint(epoch, state);
      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage(" --> [%.12lf %lf %lf...\n", epoch,
               state[0], state[1]);
      #endif
   }

   if (dupIndex > -1) {
	   #ifdef DEBUG_INTERPOLATION
		  MessageInterface::ShowMessage("1: dupEpoch: %.12lf    epoch: %.12lf    dupIndex: %d    startIndex: %d    endIndex: %d\n", dupEpoch, epoch, dupIndex, startIndex, endIndex);
	   #endif

	   if (dupEpoch > forEpoch) {
		   while (endIndex > dupIndex - 1) {
			   --startIndex;
			   --endIndex;
		   }
	   }
	   else {
		   while (startIndex < dupIndex) {
			   ++startIndex;
			   ++endIndex;
		   }
	   }
	   #ifdef DEBUG_INTERPOLATION
		  MessageInterface::ShowMessage("2: dupEpoch: %.12lf    epoch: %.12lf    dupIndex: %d    startIndex: %d    endIndex: %d\n", dupEpoch, epoch, dupIndex, startIndex, endIndex);
	   #endif

	   interp->Clear();

	   #ifdef DEBUG_INTERPOLATION
		  MessageInterface::ShowMessage("Pairs used for epoch %.12lf:\n", forEpoch);
	   #endif

	   for (UnsignedInt i = startIndex; i <= endIndex; ++i)
	   {
		   epoch = ephemStart + ephemRecords->at(i).timeFromEpoch /
			   GmatTimeConstants::SECS_PER_DAY;

		   for (UnsignedInt j = 0; j < 3; ++j)
		   {
			   state[j] = ephemRecords->at(i).theState[j];
			   state[j + 3] = ephemRecords->at(i).theState[j + 3];
		   }

		   interp->AddPoint(epoch, state);

		   #ifdef DEBUG_INTERPOLATION
		   MessageInterface::ShowMessage(" --> [%.12lf %lf %lf...\n", epoch,
			   state[0], state[1]);
		   #endif
	   }
   }
}
