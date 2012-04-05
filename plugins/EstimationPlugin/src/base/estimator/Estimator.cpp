//$Id: Estimator.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         Estimator
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
// Created: 2009/08/03
//
/**
 * Implementation of the Estimator base class
 */
//------------------------------------------------------------------------------


#include "Estimator.hpp"
#include "GmatState.hpp"
#include "PropagationStateManager.hpp"
#include "SolverException.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_STATE_MACHINE
//#define DEBUG_ESTIMATOR_WRITE
//#define DEBUG_ESTIMATOR_INITIALIZATION

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
   "AddResidualsPlot ",
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
   absoluteTolerance    (1.0e-3),
   relativeTolerance    (1.0e-3),
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
   showAllResiduals     (true),
   showSpecificResiduals(false),
   showErrorBars        (false),
   locatingEvent        (false)
{
   objectTypeNames.push_back("Estimator");
   parameterCount = EstimatorParamCount;

   esm.SetMeasurementManager(&measManager);
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
   measurementNames     (est.measurementNames),
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
   showAllResiduals     (est.showAllResiduals),
   showSpecificResiduals(est.showSpecificResiduals),
   showErrorBars        (est.showErrorBars),
   locatingEvent        (false)
{
   if (est.propagator)
      propagator = (PropSetup*)est.propagator->Clone();
   else
      propagator = NULL;

   measManager = est.measManager;
   esm         = est.esm;
   addedPlots  = est.addedPlots;

   esm.SetMeasurementManager(&measManager);
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

      measurementNames = est.measurementNames;
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
      showAllResiduals     = est.showAllResiduals;
      showSpecificResiduals= est.showSpecificResiduals;
      showErrorBars        = est.showErrorBars;
      addedPlots           = est.addedPlots;

      locatingEvent        = false;
   }

   return *this;
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
   bool retval = Solver::Initialize();

   if (retval)
   {
      // Check to make sure required objects have been set
      if (!propagator)
         throw SolverException(
               "Estimator error - no propagator set for the estimator object"
               /*" named %s.", GetName().c_str()*/);

      if (measurementNames.empty())
         throw SolverException("Estimator error - no measurements set.\n");
   }

   return retval;
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
   stm = esm.GetSTM();
   stateCovariance = esm.GetCovariance();
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
Real        Estimator::GetRealParameter(const Integer id) const
{
   if (id == ABSOLUTETOLERANCE)
      return absoluteTolerance;
   if (id == RELATIVETOLERANCE)
      return relativeTolerance;

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
      return absoluteTolerance;
   }

   if (id == RELATIVETOLERANCE)
   {
      if ((value > 0.0) && (value <= 1.0))
         relativeTolerance = value;
      return relativeTolerance;
   }

   return Solver::SetRealParameter(id, value);
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
      return measurementNames.at(index);
   if (id == SOLVEFORS)
      return solveForStrings.at(index);
   if (id == ADD_RESIDUAL_PLOT)
         return addedPlots.at(index);

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
      propagatorName = value;
      return true;
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
 * @exception <SolverException> thrown if value is out of range
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
         throw SolverException(errmsg);
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
      Integer sz = (Integer) solveForStrings.size();
      if (index == sz) // needs to be added to the end of the list
         solveForStrings.push_back(value);
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
               "into measurement array is out of bounds.\n";
         throw SolverException(errmsg);
      }
      else // is in bounds
         solveForStrings.at(index) = value;

      // Load the string into the ESM so that the object list can be built
      esm.SetProperty(value, index);

      return true;
   }

   if (id == ADD_RESIDUAL_PLOT)
   {
      Integer sz = (Integer)addedPlots.size();
      if (index == sz) // needs to be added to the end of the list
         addedPlots.push_back(value);
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
               "into residual plot array is out of bounds.\n";
         throw SolverException(errmsg);
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
         return true;
      }
   }

   StringArray measList = measManager.GetMeasurementNames();

   if (find(measList.begin(), measList.end(), name) != measList.end())
   {
      if (obj->IsOfType(Gmat::MEASUREMENT_MODEL))
      {
         measManager.AddMeasurement((MeasurementModel *)obj);
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
   return Solver::GetRefObjectArray(typeString);
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
   throw SolverException("To do: implement Estimator::UpdateClonedObject");
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
 * @return true on convergence, false if convergence was not achieved
 */
//------------------------------------------------------------------------------
bool Estimator::TestForConvergence(std::string &reason)
{
   return false;
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
      throw SolverException("Error converting the time string \"" + theEpoch +
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
 * @param plotName The name of the plot.  This name needs to ne unique in the
 *                 Sandbox
 * @param measurementNames The names of the measurement models that are sources
 *                         for the residuals being plotted
 */
//------------------------------------------------------------------------------
void Estimator::BuildResidualPlot(const std::string &plotName,
      const StringArray &measurementNames)
{
   OwnedPlot *rPlot = new OwnedPlot(plotName);

   rPlot->SetStringParameter("PlotTitle", plotName);
   rPlot->SetBooleanParameter("UseLines", false);
   rPlot->SetBooleanParameter("UseHiLow", showErrorBars);

   for (UnsignedInt i = 0; i < measurementNames.size(); ++i)
   {
      std::string curveName = measurementNames[i] + " Residuals";
      rPlot->SetStringParameter("Add", curveName);
      // Register measurement ID for this curve
      Integer id = measManager.GetMeasurementId(measurementNames[i]);

      rPlot->SetUsedDataID(id);

      // todo: Register participants for this curve
      //rPlot->SetUsedObjectID(Integer id);
   }

   rPlot->Initialize();
   residualPlots.push_back(rPlot);
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
