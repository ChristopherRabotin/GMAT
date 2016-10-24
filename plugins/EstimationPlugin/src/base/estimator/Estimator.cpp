//$Id: Estimator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         Estimator
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
// Created: 2009/08/03
//
/**
 * Implementation of the Estimator base class
 */
//------------------------------------------------------------------------------


#include "Estimator.hpp"
#include "GmatState.hpp"
#include "GmatGlobal.hpp"
#include "PropagationStateManager.hpp"
//#include "SolverException.hpp"
#include "SpaceObject.hpp"
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
#include "Spacecraft.hpp"
#include "GroundstationInterface.hpp"
#include "StringUtil.hpp"

#include <sstream>

//#define DEBUG_CONSTRUCTION
//#define DEBUG_STATE_MACHINE
//#define DEBUG_ESTIMATOR_WRITE
//#define DEBUG_ESTIMATOR_INITIALIZATION
//#define DEBUG_INITIALIZE
//#define DEBUG_CLONED_PARAMETER UPDATES

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
Estimator::PARAMETER_TEXT[] =
{
   "Measurements",
   "AddSolveFor",
   "AbsoluteTol",
   "RelativeTol",
   "Propagator",
   "ShowAllResiduals",
   "AddResidualsPlot",
   "OLSEInitialRMSSigma",
   "OLSEMultiplicativeConstant",
   "OLSEAdditiveConstant",
   "ResetBestRMSIfDiverging",
   "ConvergentStatus",
};

const Gmat::ParameterType
Estimator::PARAMETER_TYPE[] =
{
   Gmat::OBJECTARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::ON_OFF_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Estimator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The estimator type
 * @param name The name of the new Estimator
 */
//------------------------------------------------------------------------------
Estimator::Estimator(const std::string &type, const std::string &name) :
   Solver               (type, name),
   solarSystem          (NULL),
   absoluteTolerance    (1.0e-3),
   relativeTolerance    (1.0e-4),
   propagatorName       (""),
   resetState           (false),
   timeStep             (60.0),
   propagator           (NULL),
   estimationEpoch      (-1.0),
   currentEpoch         (-1.0),
   nextMeasurementEpoch (-1.0),
   stm                  (NULL),
   stateCovariance      (NULL),
   estimationState      (NULL),
   stateSize            (0),
   estimationStatus     (UNKNOWN),
   showAllResiduals     (true),
   showSpecificResiduals(false),
   showErrorBars        (false),
   locatingEvent        (false),
   maxResidualMult      (3000.0),
   constMult            (3.0),
   additiveConst        (0.0),
   resetBestRMSFlag     (false)
{

   objectTypeNames.push_back("Estimator");
   parameterCount = EstimatorParamCount;

   // Default value for Estimation.MaximumIterations = 15
   maxIterations = 15;

   esm.SetMeasurementManager(&measManager);

   delayInitialization = true;
}


//------------------------------------------------------------------------------
// ~Estimator()
//------------------------------------------------------------------------------
/**
 * Class destructor
 */
//------------------------------------------------------------------------------
Estimator::~Estimator()
{
   if (propagator)
      delete propagator;

   for (UnsignedInt i = 0; i < residualPlots.size(); ++i)
      delete residualPlots[i];
}


//------------------------------------------------------------------------------
// Estimator(const Estimator& est)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param est The estimator that is being copied
 */
//------------------------------------------------------------------------------
Estimator::Estimator(const Estimator& est) :
   Solver               (est),
   solarSystem          (est.solarSystem),
   measurementNames     (est.measurementNames),
   modelNames           (est.modelNames),
   solveForStrings      (est.solveForStrings),
   absoluteTolerance    (est.absoluteTolerance),
   relativeTolerance    (est.relativeTolerance),
   propagatorName       (est.propagatorName),
   resetState           (false),
   timeStep             (est.timeStep),
   refObjectList        (est.refObjectList),
   estimationEpoch      (est.estimationEpoch),
   currentEpoch         (est.currentEpoch),
   nextMeasurementEpoch (est.nextMeasurementEpoch),
   stm                  (NULL),
   stateCovariance      (est.stateCovariance),
   estimationState      (NULL),
   stateSize            (0),
   estimationStatus     (UNKNOWN),
   showAllResiduals     (est.showAllResiduals),
   showSpecificResiduals(est.showSpecificResiduals),
   showErrorBars        (est.showErrorBars),
   locatingEvent        (false),
   maxResidualMult      (est.maxResidualMult),
   constMult            (est.constMult),
   additiveConst        (est.additiveConst),
   resetBestRMSFlag     (est.resetBestRMSFlag)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("Estimator::Estimator() enter: <%p,%s> copy constructor from <%p,%s>\n", this, GetName().c_str(), &est, est.GetName().c_str());  
#endif

   if (est.propagator)
      propagator = (PropSetup*)est.propagator->Clone();
   else
      propagator = NULL;

   measManager = est.measManager;
   esm         = est.esm;
   addedPlots  = est.addedPlots;
   esm.SetMeasurementManager(&measManager);

   delayInitialization = true;

#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("Estimator::Estimator() exit: <%p,%s> copy constructor from <%p,%s>\n", this, GetName().c_str(), &est, est.GetName().c_str());  
#endif
}


//------------------------------------------------------------------------------
// Estimator& operator=(const Estimator& est)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param est The Estimator supplying the configuration data for this instance
 *
 * @return This instance, configured to match est
 */
//------------------------------------------------------------------------------
Estimator& Estimator::operator=(const Estimator& est)
{
   if (this != &est)
   {
      Solver::operator=(est);

      solarSystem = est.solarSystem;

      measurementNames = est.measurementNames;
      modelNames       = est.modelNames;
      solveForStrings  = est.solveForStrings;

      absoluteTolerance = est.absoluteTolerance;
      relativeTolerance = est.relativeTolerance;
      propagatorName    = est.propagatorName;

      if (est.propagator)
         propagator = (PropSetup*)est.propagator->Clone();
      else
         propagator = NULL;

      measManager = est.measManager;

      estimationEpoch      = est.estimationEpoch;
      currentEpoch         = est.currentEpoch;
      nextMeasurementEpoch = est.nextMeasurementEpoch;
      stm                  = NULL;
      covariance           = NULL;
      estimationState      = NULL;
      stateSize            = 0;
      estimationStatus     = UNKNOWN;
      showAllResiduals     = est.showAllResiduals;
      showSpecificResiduals= est.showSpecificResiduals;
      showErrorBars        = est.showErrorBars;
      addedPlots           = est.addedPlots;

      locatingEvent        = false;

      maxResidualMult      = est.maxResidualMult;
      constMult            = est.constMult;
      additiveConst        = est.additiveConst;
      resetBestRMSFlag     = est.resetBestRMSFlag;
   }

   return *this;
}


void Estimator::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the estimator - checks for unset references and does some
 * validation checking.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Enter Estimator::Initialize()\n");
#endif

   // it the delay flag is on, skip initialization to the next time
   if (delayInitialization)
   {
      #ifdef DEBUG_INITIALIZE
         MessageInterface::ShowMessage("Exit Estimator::Initialize(): delay initialization.\n");
      #endif
      return true;
   }

   // If initialized, do not do it again
   if (isInitialized)
      return true;

   bool retval = Solver::Initialize();

   if (retval)
   {
      // Set estimation status to UNKNOWN
      estimationStatus = UNKNOWN;

      // Check to make sure required objects have been set
      if (!propagator)
         throw EstimatorException(
               "Estimator error - no propagators are set for estimation or propagators are not defined in your script.\n");

      if (measurementNames.empty())
         throw EstimatorException("Error: no measurements are set for estimation.\n");


      // Check the names of measurement models shown in est.AddData have to be the names of created objects
      std::vector<MeasurementModel*> measModels = measManager.GetAllMeasurementModels();
      std::vector<TrackingSystem*> tkSystems = measManager.GetAllTrackingSystems();
      std::vector<TrackingFileSet*> tfs = measManager.GetAllTrackingFileSets();
      StringArray measNames = measManager.GetMeasurementNames();

      for(UnsignedInt i = 0; i < measNames.size(); ++i)
      {
         std::string name = measNames[i];
         bool found = false;
         for(UnsignedInt j = 0; j < measModels.size(); ++j)
         {
            if (measModels[j]->GetName() == name)
            {
               found = true;
               break;
            }
         }

         if (!found)
         {
            for(UnsignedInt j = 0; j < tkSystems.size(); ++j)
            {
               if (tkSystems[j]->GetName() == name)
               {
                  found = true;
                  break;
               }
            }
         }
      
         if (!found)
         {
            for(UnsignedInt j = 0; j < tfs.size(); ++j)
            {
               if (tfs[j]->GetName() == name)
               {
                  found = true;
                  break;
               }
            }
         }

         if (!found)
            throw EstimatorException("Cannot initialize estimator; '" + name +
                  "' object is not defined in script.\n");
      }

      measModels.clear();
      tkSystems.clear();
      tfs.clear();
      measNames.clear();

      // Get EOP time range
      GmatGlobal::Instance()->GetEopFile()->GetTimeRage(eopTimeMin, eopTimeMax);
   }

#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Exit Estimator::Initialize()   retval = %s\n", (retval?"true":"false"));
#endif

   return retval;
}


bool Estimator::Reinitialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Enter Estimator::Reinitialize()\n");
#endif
      // Tell the measManager to complete its initialization
      bool measOK = measManager.SetPropagator(propagator);
      measOK = measOK && measManager.Initialize();
      if (!measOK)
         throw EstimatorException(
           "BatchEstimator::CompleteInitialization - error initializing "
            "MeasurementManager.\n");
      
      //@todo: auto generate tracking data adapters from observation data
      //1. Read observation data from files and create a list of all tracking configs
      UnsignedInt numRec = measManager.LoadObservations();
      if (numRec == 0)
         throw EstimatorException("No observation data is used for estimation\n");

      //2. Generate tracking data adapters based on the list of tracking configs
      measManager.AutoGenerateTrackingDataAdapters();
   
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Exit Estimator::Reinitialize()\n");
#endif
   return true;
}


void Estimator::SetDelayInitialization(bool delay)
{
   delayInitialization = delay;
}


//------------------------------------------------------------------------------
// void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Completes the initialization process.
 *
 * This method is called in the INITIALIZING state of the estimator's finite
 * state machine.
 */
//------------------------------------------------------------------------------
void Estimator::CompleteInitialization()
{
   if (esm.IsPropertiesSetupCorrect())       // Verify solve-for parameters setting up correctly 
   {
      stm = esm.GetSTM();
      stateCovariance = esm.GetCovariance();
   }
}


//------------------------------------------------------------------------------
//  bool Finalize()
//------------------------------------------------------------------------------
/**
 * Finalizes the estimator.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::Finalize()
{
   bool retval = Solver::Finalize();
   return retval;
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
 *
 */
//------------------------------------------------------------------------------
std::string Estimator::GetParameterText(const Integer id) const
{
   if (id >= SolverParamCount && id < EstimatorParamCount)
      return PARAMETER_TEXT[id - SolverParamCount];
   return Solver::GetParameterText(id);
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
std::string Estimator::GetParameterUnit(const Integer id) const
{
   return Solver::GetParameterUnit(id); // TBD
}

//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer     Estimator::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverParamCount; i < EstimatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SolverParamCount])
         return i;
   }

   return Solver::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Estimator::GetParameterType(const Integer id) const
{
   if (id >= SolverParamCount && id < EstimatorParamCount)
      return PARAMETER_TYPE[id - SolverParamCount];

   return Solver::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Estimator::GetParameterTypeString(const Integer id) const
{
   return Solver::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Estimator::IsParameterReadOnly(const Integer id) const
{
   if (id == CONVERGENT_STATUS)
      return true;

   return Solver::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Estimator::GetRealParameter(const Integer id) const
{
   if (id == ABSOLUTETOLERANCE)
      return absoluteTolerance;
   if (id == RELATIVETOLERANCE)
      return relativeTolerance;
   if (id == MAX_RESIDUAL_MULTIPLIER)
      return maxResidualMult;
   if (id == CONSTANT_MULTIPLIER)
      return constMult;
   if (id == ADDITIVE_CONSTANT)
      return additiveConst;

   return Solver::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the parameter whose value to change.
 * @param value Value for the parameter.
 *
 * @return  Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Estimator::SetRealParameter(const Integer id, const Real value)
{
   if (id == ABSOLUTETOLERANCE)
   {
      if (value > 0.0)
         absoluteTolerance = value;
      else
         throw EstimatorException("Error: "+ GetName() +"."+ GetParameterText(id) +" parameter is not a positive number\n");

      return absoluteTolerance;
   }

   if (id == RELATIVETOLERANCE)
   {
      if ((value > 0.0) && (value <= 1.0))
         relativeTolerance = value;
      else
         throw EstimatorException("Error: "+ GetName() +"."+ GetParameterText(id) +" parameter is not in range (0,1]\n");

      return relativeTolerance;
   }

   if (id == MAX_RESIDUAL_MULTIPLIER)
   {
       if (value > 0.0)
           maxResidualMult = value;
      else
         throw EstimatorException("Error: "+ GetName() +"."+ GetParameterText(id) +" parameter is not a positive number\n");

       return maxResidualMult;
   }

   if (id == CONSTANT_MULTIPLIER)
   {
      if (value > 0.0)
           constMult = value;
      else
         throw EstimatorException("Error: "+ GetName() +"."+ GetParameterText(id) +" parameter is not a positive number\n");

      return constMult;
   }

   if (id == ADDITIVE_CONSTANT)
   {
      additiveConst = value;
      return additiveConst;
   }


   return Solver::SetRealParameter(id, value);
}


Real Estimator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


Real Estimator::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Estimator::GetStringParameter(const Integer id) const
{
   if (id == PROPAGATOR)
      return propagatorName;

   if (id == CONVERGENT_STATUS)
   {
      switch (estimationStatus)
      {
      case UNKNOWN:  
         return "Unknown";
         break;
      case ABSOLUTETOL_CONVERGED:
         return "Meet Absolute Tolerance convergence criteria";
         break;
      case RELATIVETOL_CONVERGED:
         return "Meet Relative Tolerance convergence criteria";
         break;
      case ABS_AND_REL_TOL_CONVERGED:
         return "Meet Absolute and Relative Tolerance convergence criteria";
         break;
      case MAX_CONSECUTIVE_DIVERGED:
         return "Exceed maximum number of consecutive diverged iterations"; 
         break;
      case MAX_ITERATIONS_DIVERGED:
         return "Exceed maximum number of iterations";
         break;
      case CONVERGING:
         return "Converging";
         break;
      case DIVERGING:
         return "Diverging";
         break;
      }
   }

   return Solver::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value stored in a StringArray, given
 * the input parameter ID and location in the array.
 *
 * @param id ID for the requested parameter.
 * @param index Location of the requested parameter in the array.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Estimator::GetStringParameter(const Integer id,
                                          const Integer index) const
{
   if (id == MEASUREMENTS)
   {
      if (((Integer)measurementNames.size() > index) && (index >= 0))
         return measurementNames[index];
      else
         throw EstimatorException("Index out of bounds when trying to access "
               "a measurement");
      
      //return measurementNames.at(index);
   }

   if (id == SOLVEFORS)
   {
      if (((Integer)solveForStrings.size() > index) && (index >= 0))
         return solveForStrings[index];
      else
         throw EstimatorException("Index out of bounds when trying to access "
               "a solve-for");

      //return solveForStrings.at(index);
   }

   if (id == ADD_RESIDUAL_PLOT)
   {
      if (((Integer)addedPlots.size() > index) && (index >= 0))
         return addedPlots[index];
      else
         throw EstimatorException("Index out of bounds when trying to access "
               "a plot");

      //return addedPlots.at(index);
   }

   return Solver::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool Estimator::SetStringParameter(const Integer id,
                                   const std::string &value) // const?
{
   if (id == PROPAGATOR)
   {
      if (!GmatStringUtil::IsValidIdentity(value))
         throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".Propagator is an invalid GMAT object name.\n");

      propagatorName = value;
      return true;
   }

   if (id == MEASUREMENTS)
   {
      std::string measName = GmatStringUtil::Trim(GmatStringUtil::RemoveOuterString(value, "{", "}"));
      if (measName == "")
         throw EstimatorException("Error: No measurement is set to " + GetName() + ".Measurements parameter.\n");

      if (!GmatStringUtil::IsValidIdentity(value))
         throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".Measurements is an invalid GMAT object name.\n");

      return SetStringParameter(id, measName, measurementNames.size());
   }


   if (id == ReportStyle)
   {
      Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
      //if ((runmode != GmatGlobal::TESTING) && (runmode != GmatGlobal::TESTING_NO_PLOTS))
      if (runmode != GmatGlobal::TESTING)
      {
         Integer i = NORMAL_STYLE;
         if (value == STYLE_TEXT[i - NORMAL_STYLE])
         {
            textFileMode = value;
            progressStyle = i;
            return true;
         }
         throw SolverException(
            "The value of \"" + value + "\" for field \"Report Style\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Normal].\n");
      }
   }

   return Solver::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID and the index.
 *
 * @param id ID for the requested parameter.
 * @param value string value for the requested parameter.
 * @param index index into the StringArray.
 *
 * @exception <EstimatorException> thrown if value is out of range
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool Estimator::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{
   if (id == MEASUREMENTS)
   {
      // an empty list is set to Measurements parameter when index == -1
      if (index == -1)
      {
         measurementNames.clear();
         return true;
      }

      // Verify measurement name
      if (!GmatStringUtil::IsValidIdentity(value))
         throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".Measurements is an invalid GMAT object name.\n");
      
      Integer sz = (Integer) measurementNames.size();
      if (index == sz) // needs to be added to the end of the list
      {
         measurementNames.push_back(value);
         measManager.AddMeasurementName(value);
      }
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
               "into measurement array is out of bounds.\n";
         throw EstimatorException(errmsg);
      }
      else // is in bounds
      {
         measurementNames.at(index) = value;
         measManager.AddMeasurementName(value);
      }
      return true;
   }

   if (id == SOLVEFORS)
   {
      // an empty list is set to SolveFors parameter when index == -1
      if (index == -1)
      {
         measurementNames.clear();
         return true;
      }

      Integer sz = (Integer) solveForStrings.size();
      if (index == sz) // needs to be added to the end of the list
         solveForStrings.push_back(value);
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
               "into measurement array is out of bounds.\n";
         throw EstimatorException(errmsg);
      }
      else // is in bounds
         solveForStrings.at(index) = value;

      // Load the string into the ESM so that the object list can be built
      esm.SetProperty(value, index);

      return true;
   }

   if (id == ADD_RESIDUAL_PLOT)
   {
      // Nothing to do when an empty list is added to AddResualPlot parameter
      if (index == -1)
         return true;

      Integer sz = (Integer)addedPlots.size();
      if (index == sz) // needs to be added to the end of the list
         addedPlots.push_back(value);
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
               "into residual plot array is out of bounds.\n";
         throw EstimatorException(errmsg);
      }
      else // is in bounds
         addedPlots.at(index) = value;

      return true;
   }

   return Solver::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter((const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& Estimator::GetStringArrayParameter(const Integer id) const
{
   if (id == MEASUREMENTS)
      return measurementNames; // temporary

   if (id == SOLVEFORS)
      return solveForStrings;

   if (id == ADD_RESIDUAL_PLOT)
      return addedPlots;

   return Solver::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves boolean parameters that are scripted using the words On and Off
 *
 * @param id The ID of the parameter
 *
 * @return the word "On" or "Off"
 */
//------------------------------------------------------------------------------
std::string Estimator::GetOnOffParameter(const Integer id) const
{
   if (id == SHOW_RESIDUALS)
      return (showAllResiduals ? "On" : "Off");

   return Solver::GetOnOffParameter(id);
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets boolean parameters that are scripted using the words On and Off
 *
 * @param id The ID of the parameter
 * @param value The new setting, "On" or "Off"
 *
 * @return true on success, false if the parameter was not set
 */
//------------------------------------------------------------------------------
bool Estimator::SetOnOffParameter(const Integer id, const std::string &value)
{
   if (id == SHOW_RESIDUALS)
   {
      if (value == "On")
      {
         showAllResiduals = true;
         return true;
      }
      else if (value == "Off")
      {
         showAllResiduals = false;
         return true;
      }

      return false;
   }

   return Solver::SetOnOffParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves boolean parameters that are scripted using the words On and Off
 *
 * @param label The string used to script the parameter
 *
 * @return The word "On" or "Off"
 */
//------------------------------------------------------------------------------
std::string Estimator::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets boolean parameters that are scripted using the words On and Off
 *
 * @param label The string used to script the parameter
 * @param value The new setting, "On" or "Off"
 *
 * @return true on success, false if the parameter was not set
 */
//------------------------------------------------------------------------------
bool Estimator::SetOnOffParameter(const std::string &label,
      const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Gets value of a boolean parameter
*
* @param id       The id of parameter
*
* @return         value of boolean parameter
*/
//------------------------------------------------------------------------------
bool Estimator::GetBooleanParameter(const Integer id) const
{
   if (id == RESET_BEST_RMS)
      return resetBestRMSFlag;

   return Solver::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
* Sets value to a boolean parameter
*
* @param id       The id of parameter
* @param value    The value used to set to a boolean parameter
*
* @return         true if the setting is successed, false otherwise
*/
//------------------------------------------------------------------------------
bool Estimator::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == RESET_BEST_RMS)
   {
      resetBestRMSFlag = value;
      return true;
   }

   return Solver::SetBooleanParameter(id, value);
}


//---------------------------------------------------------------------------
// Gmat::ObjectType GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param id ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
Gmat::ObjectType Estimator::GetPropertyObjectType(const Integer id) const
{
   if (id == MEASUREMENTS)
      return Gmat::MEASUREMENT_MODEL;

   if (id == PROPAGATOR)
      return Gmat::PROP_SETUP;

   return Solver::GetPropertyObjectType(id);
}



//------------------------------------------------------------------------------
// void UpdateCurrentEpoch(GmatEpoch newEpoch)
//------------------------------------------------------------------------------
/**
 * Sets the current epoch to a new value
 *
 * @param newEpoch The new epoch
 */
//------------------------------------------------------------------------------
void Estimator::UpdateCurrentEpoch(GmatEpoch newEpoch)
{
   currentEpoch = newEpoch;
}


//------------------------------------------------------------------------------
// GmatEpoch GetCurrentEpoch()
//------------------------------------------------------------------------------
/**
 * Retrieves the current epoch
 *
 * @return the current a.1 modified Julian epoch
 */
//------------------------------------------------------------------------------
GmatEpoch Estimator::GetCurrentEpoch()
{
   return currentEpoch;
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
bool Estimator::RenameRefObject(const Gmat::ObjectType type,
      const std::string & oldName, const std::string & newName)
{
   /// @todo Estimator rename code needs to be implemented
   return Solver::RenameRefObject(type, oldName, newName);
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
bool Estimator::SetRefObjectName(const Gmat::ObjectType type,
      const std::string & name)
{
   return Solver::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * This method retrieves am array of reference object types
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Estimator::GetRefObjectTypeArray()
{
   return Solver::GetRefObjectTypeArray();
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
const StringArray& Estimator::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   #ifdef DEBUG_ESTIMATOR_INITIALIZATION
      MessageInterface::ShowMessage(
            "Estimator::GetRefObjectNameArray(%d) entered\n", type);
   #endif

   refObjectList.clear();

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::PROP_SETUP) ||
       (type == Gmat::MEASUREMENT_MODEL))
   {
      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::PROP_SETUP))
      {
         #ifdef DEBUG_ESTIMATOR_INITIALIZATION
            MessageInterface::ShowMessage(
                  "   Adding propagator: %s\n", propagatorName.c_str());
         #endif
            if (find(refObjectList.begin(), refObjectList.end(),
                  propagatorName) == refObjectList.end())
               refObjectList.push_back(propagatorName);
      }

      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::MEASUREMENT_MODEL))
      {
         // Add the measurements this simulator needs

         for (StringArray::iterator i = measurementNames.begin();
               i != measurementNames.end(); ++i)
         {
            #ifdef DEBUG_ESTIMATOR_INITIALIZATION
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
      refObjectList = Solver::GetRefObjectNameArray(type);
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
std::string Estimator::GetRefObjectName(const Gmat::ObjectType type) const
{
   return Solver::GetRefObjectName(type);
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
GmatBase* Estimator::GetRefObject(const Gmat::ObjectType type,
      const std::string & name)
{
   return Solver::GetRefObject(type, name);
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
GmatBase* Estimator::GetRefObject(const Gmat::ObjectType type,
      const std::string & name, const Integer index)
{
   return Solver::GetRefObject(type, name, index);
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
bool Estimator::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name)
{
   #ifdef DEBUG_ESTIMATOR_INITIALIZATION
      MessageInterface::ShowMessage("Setting ref object %s with type %s\n",
            name.c_str(), obj->GetTypeName().c_str());
   #endif

   if (name == propagatorName)
   {
      if (type == Gmat::PROP_SETUP)
      {
         if (propagator != NULL)
            delete propagator;
         propagator = (PropSetup*)obj->Clone();
         measManager.SetPropagator(propagator);
         return true;
      }
   }

   StringArray measList = measManager.GetMeasurementNames();

   if (find(measList.begin(), measList.end(), name) != measList.end())
   {
      if (obj->IsOfType(Gmat::MEASUREMENT_MODEL) &&
          !(obj->IsOfType(Gmat::TRACKING_SYSTEM)))
      {
         // Handle MeasurmentModel and TrackingFileSet
         #ifdef DEBUG_ESTIMATOR_INITIALIZATION
            MessageInterface::ShowMessage("Handle MeasurementModel and TrackingFileSet: <%s>\n", name.c_str());
         #endif

         modelNames.push_back(obj->GetName());
         measManager.AddMeasurement((MeasurementModel *)obj);         
         return true;
      }
      if (obj->IsOfType(Gmat::TRACKING_SYSTEM))
      {
         // Handle for TrackingSystem
         #ifdef DEBUG_ESTIMATOR_INITIALIZATION
            MessageInterface::ShowMessage("Handle TrackingSystem: <%s>\n", name.c_str());
         #endif

         // Add to tracking system list
         measManager.AddMeasurement((TrackingSystem*)obj);
         
         MeasurementModel *meas;
         // Retrieve each measurement model from the tracking system ...
         for (UnsignedInt i = 0;
                  i < ((TrackingSystem*)obj)->GetMeasurementCount(); ++i)
         {
            #ifdef DEBUG_ESTIMATOR_INITIALIZATION
               MessageInterface::ShowMessage("   Measurement %d\n", i);
            #endif

            // ...and pass them to the measurement manager
            meas = ((TrackingSystem*)obj)->GetMeasurement(i);
            if (meas == NULL)
            {
               MessageInterface::ShowMessage("Estimator cannot initialize "
                        "because an expected MeasurementModel is NULL\n");
               throw EstimatorException("In Estimator::SetRefObject, a "
                        "measurement in the tracking system " + obj->GetName() +
                        " is NULL\n");
            }

            modelNames.push_back(meas->GetName());
            measManager.AddMeasurement(meas);

         }
         return true;
      }
   }

   return Solver::SetRefObject(obj, type, name);
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
ObjectArray& Estimator::GetRefObjectArray(const std::string & typeString)
{
   return GetRefObjectArray(GetObjectType(typeString));
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
bool Estimator::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name, const Integer index)
{
   #ifdef DEBUG_ESTIMATOR_INITIALIZATION
      MessageInterface::ShowMessage(""
            "Setting indexed ref object %s with type %s\n", name.c_str(),
            obj->GetTypeName().c_str());
   #endif

   return Solver::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// ObjectArray & GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * This method retrieves an array of reference objects of a given type
 *
 * @param type The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray & Estimator::GetRefObjectArray(const Gmat::ObjectType type)
{
   if (type == Gmat::EVENT)
   {
      activeEvents.clear();
      // Get all active events from the measurements
      activeEvents = measManager.GetActiveEvents();
      return activeEvents;
   }

   return Solver::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string & action, const std::string & actionData)
//------------------------------------------------------------------------------
/**
 * This method performs a custom action
 *
 * @param action The string defining the action
 * @param actionData Data associated with that action
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::TakeAction(const std::string & action,
      const std::string & actionData)
{
   bool retval = false;

   if (action == "ResetInstanceCount")
   {
      instanceNumber = 0;
      retval = true;
   }
   else if (action == "IncrementInstanceCount")
   {
      ++instanceNumber;
      retval = true;
   }
   else
      retval = Solver::TakeAction(action, actionData);

   return retval;
}


//------------------------------------------------------------------------------
//  Real GetPropagator()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the PropSetup object.
 *
 * @return The PropSetup pointer
 */
//------------------------------------------------------------------------------
PropSetup* Estimator::GetPropagator()
{
   return propagator;
}


//------------------------------------------------------------------------------
//  MeasurementManager* GetMeasurementManager()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the MeasurmentManager object.
 *
 * @return The MeasurementManager pointer
 */
//------------------------------------------------------------------------------
MeasurementManager* Estimator::GetMeasurementManager()
{
   return &measManager;
}


//------------------------------------------------------------------------------
// EstimationStateManager* GetEstimationStateManager()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the EstimationStateManager object.
 *
 * @return The EstimationStateManager pointer
 */
//------------------------------------------------------------------------------
EstimationStateManager* Estimator::GetEstimationStateManager()
{
   return &esm;
}


//------------------------------------------------------------------------------
//  Real GetTimeStep()
//------------------------------------------------------------------------------
/**
 * Returns the time step of the estimator.
 *
 * @return The time step
 */
//------------------------------------------------------------------------------
Real Estimator::GetTimeStep()
{
   return timeStep;
}


//------------------------------------------------------------------------------
// bool ResetState()
//------------------------------------------------------------------------------
/**
 * This method returns the resetState flag, turning it off in the process
 *
 * @return The resetState flag
 */
//------------------------------------------------------------------------------
bool Estimator::ResetState()
{
   bool retval = resetState;

   if (resetState == true)
      resetState = false;

   return retval;
}




//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
//------------------------------------------------------------------------------
bool Estimator::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Updates cloned objects that are copies of the object passed in
 *
 * @param obj The object with setting updates
 */
//------------------------------------------------------------------------------
void Estimator::UpdateClonedObject(GmatBase *obj)
{
   if (obj->IsOfType("Spacecraft"))
      return;
   throw EstimatorException("To do: implement Estimator::UpdateClonedObject "
         "for " + obj->GetTypeName() + " objects");
}

//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
 * Added method to remove message in the Message window.
 *
 * The current implementation needs to be updated to actually process parameters
 * when they are updated in the system.  For now, it is just overriding the base
 * class "do nothing" method so that the message traffic is not shown to the
 * user.
 *
 * Turn on the debug to figure out the updates being requested.
 *
 * @param obj The master object holding the new parameter value
 * @param updatedParameterId The ID of the updated parameter
 */
//------------------------------------------------------------------------------
void Estimator::UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
{
#ifdef DEBUG_CLONED_PARAMETER_UPDATES
   MessageInterface::ShowMessage("Estimator updating parameter %d (%s) using "
         "object %s\n", updatedParameterId, obj->GetParameterText(updatedParameterId).c_str(),
         obj->GetName().c_str());
#endif
}


//------------------------------------------------------------------------------
// bool TestForConvergence(std::string &reason)
//------------------------------------------------------------------------------
/**
 * Method that tests an estimation to see if the process has converged.  Derived
 * classes implement this method to provide convergence tests.  This default
 * version always returns false.
 *
 * @param reason String indicating the condition(s) that indicate convergence
 *
 * @return return an integer to tell status of estimation
 */
//------------------------------------------------------------------------------
Integer Estimator::TestForConvergence(std::string &reason)
{
    return UNKNOWN;
}

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
 * @return The converted epoch
 */
//------------------------------------------------------------------------------
Real Estimator::ConvertToRealEpoch(const std::string &theEpoch,
                                   const std::string &theFormat)
{
   Real fromMjd = -999.999;
   Real retval = -999.999;
   std::string outStr;

   TimeConverterUtil::Convert(theFormat, fromMjd, theEpoch, "A1ModJulian",
         retval, outStr);

   if (retval == -999.999)
      throw EstimatorException("Error converting the time string \"" + theEpoch +
            "\"; please check the format for the input string.");
   return retval;
}


//------------------------------------------------------------------------------
// void BuildResidualPlot(const std::string &plotName,
//       const StringArray &measurementNames)
//------------------------------------------------------------------------------
/**
 * Creates an OwnedPlot instance that is used for plotting residuals
 *
 * @param plotName The name of the plot.  This name needs to be unique in the
 *                 Sandbox
 * @param measurementNames The names of the measurement models that are sources
 *                         for the residuals being plotted
 */
//------------------------------------------------------------------------------
void Estimator::BuildResidualPlot(const std::string &plotName,
      const StringArray &measurementNames)
{
   OwnedPlot *rPlot;
   for (UnsignedInt i = 0; i < measurementNames.size(); ++i)
   {
      // Register measurement ID for this curve
      IntegerArray id = measManager.GetMeasurementId(measurementNames[i]);
      //std::vector<TrackingFileSet*> tfs = measManager.GetTrackingSets();
      std::vector<TrackingFileSet*> tfs = measManager.GetAllTrackingFileSets();

      UnsignedInt k;
      for (k = 0; k < tfs.size(); ++k)
         if (tfs[k]->GetName() == measurementNames[i])
            break;

      if (k == tfs.size())
      {
         // processing for old measurement model
         IntegerArray id = measManager.GetMeasurementId(measurementNames[i]);

         rPlot = new OwnedPlot(plotName);
         rPlot->SetStringParameter("PlotTitle", plotName);
         rPlot->SetBooleanParameter("UseLines", false);
         rPlot->SetBooleanParameter("UseHiLow", showErrorBars);
         rPlot->SetStringParameter("Add", measurementNames[i] + " Residuals");
         rPlot->SetUsedDataID(id[0]);
         rPlot->Initialize();

         residualPlots.push_back(rPlot);
      }
      else
      {
         // processing for tracking data adapter 
         std::vector<TrackingDataAdapter*>* adapters = tfs[k]->GetAdapters(); 
         for (UnsignedInt j = 0; j < adapters->size(); ++j)
         {
            Integer id = adapters->at(j)->GetModelID();

            std::stringstream ss;
            ss << adapters->at(j)->GetName();
            std::string pName = ss.str();

            std::stringstream ss1;
            ss1 << pName << " Residuals";
            std::string curveName = ss1.str();
            

            rPlot = new OwnedPlot(pName);
            rPlot->SetStringParameter("PlotTitle", plotName);
            rPlot->SetBooleanParameter("UseLines", false);
            rPlot->SetBooleanParameter("UseHiLow", showErrorBars);
            rPlot->SetStringParameter("Add", curveName);
            rPlot->SetUsedDataID(id);
            rPlot->Initialize();

            residualPlots.push_back(rPlot);
         }
      }
   }

}


//------------------------------------------------------------------------------
// void PlotResiduals()
//------------------------------------------------------------------------------
/**
 * Plots residuals by passing the residual data to the OwnedPlot objects that
 * display the data.
 */
//------------------------------------------------------------------------------
void Estimator::PlotResiduals()
{
   #ifdef DEBUG_RESIDUAL_PLOTS
      MessageInterface::ShowMessage("Entered PlotResiduals\n");
      MessageInterface::ShowMessage("Processing plot with %d Residuals\n",
            measurementResiduals.size());
   #endif

   std::vector<RealArray*> dataBlast;
   RealArray epochs;
   RealArray values;
   RealArray hiErrors;
   RealArray lowErrors;

   RealArray *hi = NULL, *low = NULL;

   for (UnsignedInt i = 0; i < residualPlots.size(); ++i)
   {
      dataBlast.clear();
      epochs.clear();
      values.clear();

      if (showErrorBars)
      {
         hiErrors.clear();
         lowErrors.clear();
         if (hiLowData.size() > 0)
         {
            hi = hiLowData[0];
            if (hiLowData.size() > 1)
            {
               low = hiLowData[1];
            }
         }
      }

      // Collect residuals by plot
      for (UnsignedInt j = 0; j < measurementResiduals.size(); ++j)
      {
         if (residualPlots[i]->UsesData(measurementResidualID[j]) >= 0)
         {
            epochs.push_back(measurementEpochs[j]);
            values.push_back(measurementResiduals[j]);
            if (hi && showErrorBars)
            {
               hiErrors.push_back((*hi)[j]);
            }
            if (low && showErrorBars)
               lowErrors.push_back((*low)[j]);
         }
      }

      if (epochs.size() > 0)
      {
         dataBlast.push_back(&epochs);
         dataBlast.push_back(&values);

         residualPlots[i]->TakeAction("ClearData");
         residualPlots[i]->Deactivate();
         residualPlots[i]->SetData(dataBlast, hiErrors, lowErrors);
         residualPlots[i]->TakeAction("Rescale");
         residualPlots[i]->Activate();
      }

      #ifdef DEBUG_RESIDUALS
         // Dump the data to screen
         MessageInterface::ShowMessage("DataDump for residuals plot %d:\n", i);
         for (UnsignedInt k = 0; k < epochs.size(); ++k)
         {
            MessageInterface::ShowMessage("   %.12lf  %.12lf", epochs[k], values[k]);

            if (hi)
               if (hi->size() > k)
                  if (low)
                     MessageInterface::ShowMessage("   + %.12lf", (*hi)[k]);
                  else
                     MessageInterface::ShowMessage("   +/- %.12lf", (*hi)[k]);
            if (low)
               if (low->size() > k)
                  MessageInterface::ShowMessage(" - %.12lf", (*low)[k]);
            MessageInterface::ShowMessage("\n");
         }
      #endif
   }
}

//------------------------------------------------------------------------------
// void EnhancePlot()
//------------------------------------------------------------------------------
/**
 * Adds decorations to residuals plots
 *
 * This method is used to add new information to the residuals plots, like
 * deviation curves, notations, and so on.
 *
 * This default method provides no enhancements
 */
//------------------------------------------------------------------------------
void Estimator::EnhancePlot()
{
}


//------------------------------------------------------------------------------
// unused methods
//------------------------------------------------------------------------------

// Methods required by base classes
// Not needed for simulation

//------------------------------------------------------------------------------
// Integer SetSolverResults(Real*, const std::string&, const std::string&)
//------------------------------------------------------------------------------
/**
 * Method used to report values generated in a SolverControlSequence.
 *
 * This method is not used in GMAT's current estimators; it is required to
 * satisfy Solver abstract method requirements.
 */
//------------------------------------------------------------------------------
Integer Estimator::SetSolverResults(Real*, const std::string&,
      const std::string&)
{
   return -1;
}


//------------------------------------------------------------------------------
// void SetResultValue(Integer, Real, const std::string&)
//------------------------------------------------------------------------------
/**
 * Method used to register contributors to that provide generated data.
 *
 * This method is not used in GMAT's current estimators; it is required to
 * satisfy Solver abstract method requirements.
 */
//------------------------------------------------------------------------------
void Estimator::SetResultValue(Integer, Real, const std::string&)
{
}


//------------------------------------------------------------------------------
// bool Estimator::ConvertToParticipantCoordSystem(ListItem* infor, Real epoch, 
//                    Real inputStateElement, Real* outputStateElement)
//------------------------------------------------------------------------------
/**
 * Method used to convert result of a state's element in A1mjd to participant's 
 * coordinate system
 *
 * @param infor                 information about state's element
 * @param epoch                 the epoch at which the state is converted it's 
 *                              coordinate system
 * @param inputStateElement     state's element in GMAT internal coordinate system
 *                              (A1Mjd)
 * @param outputStateElemnet    state's element in participant's coordinate system
 *
*/
//------------------------------------------------------------------------------
bool Estimator::ConvertToParticipantCoordSystem(ListItem* infor, Real epoch, Real inputStateElement, Real* outputStateElement)
{

   (*outputStateElement) = inputStateElement;

   if (infor->object->IsOfType(Gmat::SPACEOBJECT))
   {
      if ((infor->elementName == "CartesianState")||(infor->elementName == "Position")||(infor->elementName == "Velocity"))
      {
         SpaceObject* obj = (SpaceObject*) (infor->object);
         std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
         CoordinateSystem* cs = (CoordinateSystem*) obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
         if (cs == NULL)
            throw EstimatorException("Coordinate system for "+obj->GetName()+" is not set\n");

         SpacePoint* sp = obj->GetJ2000Body();
         CoordinateSystem* gmatcs = CoordinateSystem::CreateLocalCoordinateSystem("bodyInertial",
            "MJ2000Eq", sp, NULL, NULL, sp, cs->GetSolarSystem());
        
         CoordinateConverter* cv = new CoordinateConverter();
         Rvector6 inState(0.0,0.0,0.0,0.0,0.0,0.0);
         Integer index;
         if ((infor->elementName == "CartesianState")||(infor->elementName == "Position"))
            index = infor->subelement-1;
         else if (infor->elementName == "Velocity")
            index = infor->subelement+2;
         else
            throw EstimatorException("Error in Estimator object: Parameter %s has not defined in GMAT\n");

         inState.SetElement(index, inputStateElement);
         Rvector6 outState;
        
         cv->Convert(A1Mjd(epoch), inState, gmatcs, outState, cs);

         (*outputStateElement) = outState[index]; 
         delete cv;
         delete gmatcs;
      }
   }

   return true;
}


//-------------------------------------------------------------------------
// void Estimator::GetEstimationState(GmatState& outputState)
//-------------------------------------------------------------------------
/**
 * This Method used to convert result of estimation state to participants'
 * coordinate system
 *
 * @param outState        estimation state in participants' coordinate systems
 *
*/
//-------------------------------------------------------------------------
void Estimator::GetEstimationState(GmatState& outputState)
{
    const std::vector<ListItem*> *map = esm.GetStateMap();

    Real outputStateElement;
    outputState.SetSize(map->size());

    for (UnsignedInt i = 0; i < map->size(); ++i)
    {
        ConvertToParticipantCoordSystem((*map)[i], estimationEpoch, (*estimationState)[i], &outputStateElement);
        outputState[i] = outputStateElement;
    }
}


//-------------------------------------------------------------------------
// void Estimator::GetEstimationStateForReport(GmatState& outputState)
//-------------------------------------------------------------------------
/**
 * This Method used to convert result of estimation state to participants'
 * coordinate system. For report, it reports Cr and Cd instead of Cr_Epsilon 
 * and Cd_Epsilon.
 *
 * @param outState        estimation state in participants' coordinate systems
 *
*/
//-------------------------------------------------------------------------
void Estimator::GetEstimationStateForReport(GmatState& outputState)
{
    const std::vector<ListItem*> *map = esm.GetStateMap();

    Real outputStateElement;
    outputState.SetSize(map->size());

    for (UnsignedInt i = 0; i < map->size(); ++i)
    {
        ConvertToParticipantCoordSystem((*map)[i], estimationEpoch, (*estimationState)[i], &outputStateElement);
        outputState[i] = outputStateElement;

        // get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
        if ((*map)[i]->elementName == "Cr_Epsilon")
           outputState[i] = ((Spacecraft*) (*map)[i]->object)->GetRealParameter("Cr");
        else if ((*map)[i]->elementName == "Cd_Epsilon")
           outputState[i] = ((Spacecraft*) (*map)[i]->object)->GetRealParameter("Cd");
    }
}

