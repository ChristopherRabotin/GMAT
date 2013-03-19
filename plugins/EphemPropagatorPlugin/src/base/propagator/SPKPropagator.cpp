//$Id$
//------------------------------------------------------------------------------
//                             SPKPropagator
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
 * Implementation for the SPKPropagator class
 */
//------------------------------------------------------------------------------


#include "SPKPropagator.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include <sstream>                 // for stringstream

//#define DEBUG_INITIALIZATION
//#define DEBUG_PROPAGATION
//#define TEST_TDB_ROUND_TRIP

//---------------------------------
// static data
//---------------------------------

/// SPKPropagator parameter labels
const std::string SPKPropagator::PARAMETER_TEXT[
                 SPKPropagatorParamCount - EphemerisPropagatorParamCount] =
{
      "SPKFiles"                    //SPKFILENAMES
};

/// SPKPropagator parameter types
const Gmat::ParameterType SPKPropagator::PARAMETER_TYPE[
                 SPKPropagatorParamCount - EphemerisPropagatorParamCount] =
{
      Gmat::STRINGARRAY_TYPE        //SPKFILENAMES
};


//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// SPKPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the object that gets constructed
 */
//------------------------------------------------------------------------------
SPKPropagator::SPKPropagator(const std::string &name) :
   EphemerisPropagator        ("SPK", name),
   skr                        (NULL)
{
   // GmatBase data
  objectTypeNames.push_back("SPK");
  parameterCount       = SPKPropagatorParamCount;

  spkCentralBody       = centralBody;
  spkCentralBodyNaifId = SpiceInterface::DEFAULT_NAIF_ID;
}


//------------------------------------------------------------------------------
// ~SPKPropagator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SPKPropagator::~SPKPropagator()
{
   if (skr)
   {
      // unload the SPK kernels so they will not be retained in the kernel
      // pool
      skr->UnloadKernels(spkFileNames);
      delete skr;
   }
}


//------------------------------------------------------------------------------
// SPKPropagator(const SPKPropagator & spk)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param spk The object that is copied into this new one
 */
//------------------------------------------------------------------------------
SPKPropagator::SPKPropagator(const SPKPropagator & spk) :
   EphemerisPropagator        (spk),
   spkCentralBody             (spk.spkCentralBody),
   spkCentralBodyNaifId       (spk.spkCentralBodyNaifId),
   skr                        (NULL)
{
}


//------------------------------------------------------------------------------
// SPKPropagator & SPKPropagator::operator =(const SPKPropagator & spk)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param spk The object that is provides data for into this one
 *
 * @return This propagator, configured to match spk.
 */
//------------------------------------------------------------------------------
SPKPropagator & SPKPropagator::operator =(const SPKPropagator & spk)
{
   if (this != &spk)
   {
      EphemerisPropagator::operator=(spk);

      skr                  = NULL;
      spkCentralBody       = spk.spkCentralBody;
      spkCentralBodyNaifId = spk.spkCentralBodyNaifId;
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
GmatBase* SPKPropagator::Clone() const
{
   return new SPKPropagator(*this);
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
std::string SPKPropagator::GetParameterText(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < SPKPropagatorParamCount)
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
Integer SPKPropagator::GetParameterID(const std::string &str) const
{
   for (Integer i = EphemerisPropagatorParamCount;
         i < SPKPropagatorParamCount; ++i)
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
Gmat::ParameterType SPKPropagator::GetParameterType(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < SPKPropagatorParamCount)
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
std::string SPKPropagator::GetParameterTypeString(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < SPKPropagatorParamCount)
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
std::string SPKPropagator::GetParameterUnit(const Integer id) const
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
bool SPKPropagator::IsParameterReadOnly(const Integer id) const
{
   if (id == SPKFILENAMES)
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
bool SPKPropagator::IsParameterReadOnly(const std::string &label) const
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
std::string SPKPropagator::GetStringParameter(const Integer id) const
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
bool SPKPropagator::SetStringParameter(const Integer id,
      const std::string &value)
{

   if (id == SPKFILENAMES)
   {
      if (value != "")
         if (find(spkFileNames.begin(), spkFileNames.end(), value) ==
               spkFileNames.end())
            spkFileNames.push_back(value);
      return true;         // Idempotent, so return true
   }

   bool retval = EphemerisPropagator::SetStringParameter(id, value);

   if ((retval = true) && (id == EPHEM_CENTRAL_BODY))
   {
      // Special case code that we may want to remove later
      if (value == "Moon")
         throw PropagatorException("\"Moon\" is not an allowed central body; "
               "try \"Luna\"", Gmat::ERROR_);
      if (centralBody == "Luna")
         spkCentralBody = "Moon";
      else
         spkCentralBody = centralBody;
   }

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
std::string SPKPropagator::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == SPKFILENAMES)
   {
      if ((index >= 0) && (index < (Integer)spkFileNames.size()))
         return spkFileNames[index];
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
bool SPKPropagator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   if (id == SPKFILENAMES)
   {
      if ((index >= 0) && (index < (Integer)spkFileNames.size()))
      {
         spkFileNames[index] = value;
         return true;
      }
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
const StringArray& SPKPropagator::GetStringArrayParameter(
      const Integer id) const
{
   if (id == SPKFILENAMES)
      return spkFileNames;
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
const StringArray& SPKPropagator::GetStringArrayParameter(const Integer id,
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
std::string SPKPropagator::GetStringParameter(const std::string &label) const
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
bool SPKPropagator::SetStringParameter(const std::string &label,
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
std::string SPKPropagator::GetStringParameter(const std::string &label,
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
bool SPKPropagator::SetStringParameter(const std::string &label,
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
const StringArray& SPKPropagator::GetStringArrayParameter(
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
const StringArray& SPKPropagator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the SPKPropagator for use in a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SPKPropagator::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SPKPropagator::Initialize() entered\n");
      MessageInterface::ShowMessage("spkCentralBody is %s\n", spkCentralBody.c_str());
   #endif

   bool retval = false;

   if (EphemerisPropagator::Initialize())
   {
      // If skr already set, just keep it
      if (skr == NULL)
         skr = new SpiceOrbitKernelReader;

      spkCentralBodyNaifId = skr->GetNaifID(spkCentralBody);

      stepTaken = 0.0;
      j2ET = j2000_c();   // CSPICE method to return Julian date of J2000 (TDB)

      FileManager *fm = FileManager::Instance();
      std::string fullPath = fm->GetFullPathname(FileManager::PLANETARY_SPK_FILE);

      if (skr->IsLoaded(fullPath) == false)
         skr->LoadKernel(fullPath);

      if (propObjects.size() != 1)
         throw PropagatorException("SPICE propagators (i.e. \"SPK\" "
               "propagators) require exactly one SpaceObject.");
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Clearing %d naifIds\n", naifIds.size());
      #endif
      naifIds.clear();

      // The PSM isn't set until PrepareToPropagate fires.  The following is
      // also last minute setup, so only do it if the PSM has been set
      if (psm != NULL)
      {
         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
            Integer id = propObjects[i]->GetIntegerParameter("NAIFId");
            naifIds.push_back(id);

            // Load the SPICE files for each propObject
            StringArray spices;
            if (propObjects[i]->IsOfType(Gmat::SPACECRAFT))
               spices = propObjects[i]->GetStringArrayParameter(
                     "OrbitSpiceKernelName");
            else
               throw PropagatorException("Spice (SPK) propagators only work for "
                     "Spacecraft right now.");

            if (spices.size() == 0)
               throw PropagatorException("Spice (SPK) propagator requires at "
                     "least one orbit SPICE kernel,");

            std::string ephemPath = fm->GetPathname(FileManager::EPHEM_PATH);
            for (UnsignedInt j = 0; j < spices.size(); ++j)
            {
               fullPath = spices[j];

               // Check to see if this name includes path information
               // If no path designation slash character is found, add the default path
               if ((fullPath.find('/') == std::string::npos) &&
                   (fullPath.find('\\') == std::string::npos))
               {
                  fullPath = ephemPath + fullPath;
               }
               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Checking for kernel %s\n",
                        fullPath.c_str());
               #endif
               if (skr->IsLoaded(fullPath) == false)
                  skr->LoadKernel(fullPath);

               if (find(spkFileNames.begin(), spkFileNames.end(), fullPath) ==
                     spkFileNames.end())
                  spkFileNames.push_back(fullPath);
            }

            // Load the initial data point
            if (skr)
            {
               try
               {
                  Rvector6  outState;

                  for (UnsignedInt i = 0; i < propObjects.size(); ++i)
                  {
                     std::string scName = propObjectNames[i];
                     Integer id = naifIds[i];

                     currentEpoch = initialEpoch + timeFromEpoch /
                           GmatTimeConstants::SECS_PER_DAY;

                     // Allow for slop in the last few bits
                     if ((currentEpoch < ephemStart - 1e-10) ||
                         (currentEpoch > ephemEnd + 1e-10))
                     {
                        std::stringstream errmsg;
                        errmsg.precision(16);
                        errmsg << "The SPKPropagator "
                               << instanceName
                               << " is attempting to initialize outside of the "
                                  "timespan  of the ephemeris data; halting.  ";
                        errmsg << "The current SPICE ephemeris covers the A.1 "
                                  "modified Julian span ";
                        errmsg << ephemStart << " to " << ephemEnd << " and the "
                                 "requested epoch is " << currentEpoch << ".\n";
                        throw PropagatorException(errmsg.str());
                     }
                     #ifdef DEBUG_INITIALIZATION
                        MessageInterface::ShowMessage("Getting target state in %p "
                              "for %s (ID = %ld) at epoch %lf and CB %s\n", this,
                              scName.c_str(), id, currentEpoch,
                              spkCentralBody.c_str());
                     #endif
                     outState = skr->GetTargetState(scName, id, currentEpoch,
                           spkCentralBody, spkCentralBodyNaifId);

                     std::memcpy(state, outState.GetDataVector(),
                           dimension*sizeof(Real));
                  }

                  UpdateSpaceObject(currentEpoch);

                  retval = true;
               }
               catch (BaseException &e)
               {
                  MessageInterface::ShowMessage(e.GetFullMessage());
                  retval = false;
                  throw;
               }
            }
         }

         SetEphemSpan();
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SPKPropagator::Initialize(): Start state "
            "at epoch %.12lf is [", currentEpoch);
      for (Integer i = 0; i < dimension; ++i)
      {
         MessageInterface::ShowMessage("%.12lf", state[i]);
         if (i < dimension-1)
            MessageInterface::ShowMessage("   ");
         else
            MessageInterface::ShowMessage("]\n");
      }
      MessageInterface::ShowMessage("SPKPropagator::Initialize() finished\n");
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
bool SPKPropagator::Step()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("SPKPropagator::Step() entered: "
            "initialEpoch = %.12lf; stepsize = %.12lf; "
            "timeFromEpoch = %.12lf\n", initialEpoch, ephemStep, timeFromEpoch);
   #endif

   bool retval = false;

   if (skr)
   {
      try
      {
         Rvector6  outState;

         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
            std::string scName = propObjectNames[i];
            Integer id = naifIds[i];

            timeFromEpoch += ephemStep;
            stepTaken = ephemStep;
            currentEpoch = initialEpoch + timeFromEpoch /
                  GmatTimeConstants::SECS_PER_DAY;

            // Allow for slop in the last few bits
            if ((currentEpoch < ephemStart - 1e-10) ||
                (currentEpoch > ephemEnd + 1e-10))
            {
               std::stringstream errmsg;
               errmsg.precision(16);
               errmsg << "The SPKPropagator "
                      << instanceName
                      << " is attempting to step outside of the span of the "
                         "ephemeris data; halting.  ";
               errmsg << "The current SPICE ephemeris covers the A.1 modified "
                         "Julian span ";
               errmsg << ephemStart << " to " << ephemEnd << " and the "
                     "requested epoch is " << currentEpoch << ".\n";
               throw PropagatorException(errmsg.str());
            }

            outState = skr->GetTargetState(scName, id, currentEpoch,
                  spkCentralBody, spkCentralBodyNaifId);

            /**
             *  @todo: When SPKProp can evolve more than one spacecraft, these
             *  memcpy lines need revision
             */
//            std::memcpy(state, outState.GetDataVector(),
//                  dimension*sizeof(Real));
//            ReturnFromOrigin(currentEpoch);
//            std::memcpy(j2kState, outState.GetDataVector(),
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
         }

         retval = true;
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage(e.GetFullMessage());
         retval = false;
         throw;
      }
   }

   #ifdef DEBUG_PROPAGATION
      else
         MessageInterface::ShowMessage("skr was not initialized]\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool RawStep()
//------------------------------------------------------------------------------
/**
 * Performs a propagation step without error control
 *
 * @note: RawStep is not used with the SPKPropagator
 *
 * @return false always
 */
//------------------------------------------------------------------------------
bool SPKPropagator::RawStep()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Real GetStepTaken()
//------------------------------------------------------------------------------
/**
 * Retrieves the size of the most recent SPKPropagator step
 *
 * @return The most recent step (0.0 if no step was taken with this instance).
 */
//------------------------------------------------------------------------------
Real SPKPropagator::GetStepTaken()
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
void SPKPropagator::UpdateState()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Updating state to epoch %.12lf\n",
            currentEpoch);
   #endif

   if (skr)
   {
      try
      {
         Rvector6  outState;

         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
            std::string scName = propObjectNames[i];
            Integer id = naifIds[i];

            // Allow for slop in the last few bits
            if ((currentEpoch < ephemStart - 1e-10) ||
                (currentEpoch > ephemEnd + 1e-10))
            {
               std::stringstream errmsg;
               errmsg.precision(16);
               errmsg << "The SPKPropagator "
                      << instanceName
                      << " is attempting to access state data outside of the "
                         "span of the ephemeris data; halting.  ";
               errmsg << "The current SPICE ephemeris covers the A.1 modified "
                         "Julian span "
                      << ephemStart << " to " << ephemEnd << " and the "
                         "requested epoch is " << currentEpoch << ".\n";
               throw PropagatorException(errmsg.str());
            }

            outState = skr->GetTargetState(scName, id, currentEpoch,
                  spkCentralBody, spkCentralBodyNaifId);

            /**
             *  @todo: When SPKProp can evolve more than one spacecraft, this
             *  memcpy line needs revision
             */
//            std::memcpy(state, outState.GetDataVector(),
//                  dimension*sizeof(Real));
//            std::memcpy(j2kState, outState.GetDataVector(),
            std::memcpy(state, outState.GetDataVector(),
                  dimension*sizeof(Real));
//            MoveToOrigin(currentEpoch);
//            UpdateSpaceObject(currentEpoch);

            #ifdef DEBUG_PROPAGATION
               MessageInterface::ShowMessage("(UpdateState for %p) State at "
                     "epoch %.12lf is [", this, currentEpoch);
               for (Integer i = 0; i < dimension; ++i)
               {
                  MessageInterface::ShowMessage("%.12lf", state[i]);
                  if (i < 5)
                     MessageInterface::ShowMessage("   ");
                  else
                     MessageInterface::ShowMessage("]\n");
               }
            #endif
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage(e.GetFullMessage());
         throw;
      }
   }
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
void SPKPropagator::SetEphemSpan(Integer whichOne)
{
   if (whichOne < 0)
      throw PropagatorException("SPKPropagator::SetEphemSpan(Integer whichOne):"
            " Invalid index");

   if (skr)
   {
      // @todo: When the SPKPropagator supports more than one spacecraft, the
      // ephem span needs to be modified to track spans for each spacecraft
      for (UnsignedInt i = 0; i < naifIds.size(); ++i)
         skr->GetCoverageStartAndEnd(spkFileNames, naifIds[i], ephemStart,
               ephemEnd);

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("EphemSpan is [%.12lf %.12lf]\n",
               ephemStart, ephemEnd);
      #endif
   }
}
