//$Id: Simulator.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         Simulator
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
// Author: Darrel J. Conway, Thinking Systems, Inc. & Wendy Shoan/GSFC/GSSB
// Created: 2009/06/30
//
/**
 * Definition for the class used to generate simulated measurement data
 */
//------------------------------------------------------------------------------

#include "Simulator.hpp"
#include "EstimatorException.hpp"
#include "GmatState.hpp"
#include "PropagationStateManager.hpp"
#include "SolverException.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_STATE_MACHINE
//#define DEBUG_SIMULATOR_WRITE
//#define DEBUG_SIMULATOR_INITIALIZATION


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
Simulator::PARAMETER_TEXT[SimulatorParamCount -SolverParamCount] =
{
   "AddData",
   "Propagator",
   "InitialEpochFormat",
   "InitialEpoch",
   "FinalEpochFormat",
   "FinalEpoch",
   "MeasurementTimeStep",
};

const Gmat::ParameterType
Simulator::PARAMETER_TYPE[SimulatorParamCount - SolverParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::STRING_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  Simulator()  <default constructor>
//------------------------------------------------------------------------------
/**
 * This is the default constructor for the Simulator class.
 */
//------------------------------------------------------------------------------
Simulator::Simulator(const std::string& name) :
   Solver              ("Simulator", name),
   propagator          (NULL),
   propagatorName      (""),
   simState            (NULL),
   simulationStart     (GmatTimeConstants::MJD_OF_J2000),
   simulationEnd       (GmatTimeConstants::MJD_OF_J2000 + 1.0),
   nextSimulationEpoch (GmatTimeConstants::MJD_OF_J2000),
   currentEpoch        (GmatTimeConstants::MJD_OF_J2000),
   initialEpochFormat  ("TAIModJulian"),
   finalEpochFormat    ("TAIModJulian"),
   simulationStep      (60.0),
   locatingEvent       (false),
   timeStep            (60.0)
{
   objectTypeNames.push_back("Simulator");
   std::stringstream ss("");
   ss << GmatTimeConstants::MJD_OF_J2000;
   initialEpoch = ss.str();
   finalEpoch   = ss.str();
   parameterCount = SimulatorParamCount;
}


//------------------------------------------------------------------------------
//  Simulator(const Simulator &sim)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Simulator class as a copy of the
 * specified Simulator class (copy constructor).
 *
 * @param sim Simulator object to copy.
 */
//------------------------------------------------------------------------------
Simulator::Simulator(const Simulator& sim) :
   Solver              (sim),
   propagatorName      (sim.propagatorName),
   simState            (NULL),   // should this be cloned?
   simulationStart     (sim.simulationStart),
   simulationEnd       (sim.simulationEnd),
   nextSimulationEpoch (sim.nextSimulationEpoch),
   currentEpoch        (sim.currentEpoch),
   initialEpochFormat  (sim.initialEpochFormat),
   initialEpoch        (sim.initialEpoch),
   finalEpochFormat    (sim.finalEpochFormat),
   finalEpoch          (sim.finalEpoch),
   simulationStep      (sim.simulationStep),
   locatingEvent       (false),
   timeStep            (sim.timeStep),
   measManager         (sim.measManager),
   measList            (sim.measList)
{
   propagator = NULL;
   if (sim.propagator) propagator = ((PropSetup*) (sim.propagator)->Clone());
}


//------------------------------------------------------------------------------
//  Simulator& operator= (const Simulator& sim)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Simulator class.
 *
 * @param sim the Simulator object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Simulator with data of input Simulator sim.
 */
//------------------------------------------------------------------------------
Simulator& Simulator::operator =(const Simulator& sim)
{
   if (&sim != this)
   {
      Solver::operator=(sim);

      if (propagator != NULL)
         delete propagator;
      if (sim.propagator) propagator          = ((PropSetup*)
                                                    (sim.propagator)->Clone());
      else                propagator          = NULL;
      propagatorName      = sim.propagatorName;
      simState            = NULL;   // or clone it here??
      simulationStart     = sim.simulationStart;
      simulationEnd       = sim.simulationEnd;
      nextSimulationEpoch = sim.nextSimulationEpoch;
      currentEpoch        = sim.currentEpoch;
      initialEpochFormat  = sim.initialEpochFormat;
      initialEpoch        = sim.initialEpoch;
      finalEpochFormat    = sim.finalEpochFormat;
      finalEpoch          = sim.finalEpoch;
      simulationStep      = sim.simulationStep;
      timeStep            = sim.timeStep;
      locatingEvent       = false;
      measManager         = sim.measManager;
      measList            = sim.measList;
   }

   return *this;
}


//------------------------------------------------------------------------------
//  ~Simulator()
//------------------------------------------------------------------------------
/**
 * Destructor for the Simulator class.
 */
//------------------------------------------------------------------------------
Simulator::~Simulator()
{
   if (propagator)
      delete propagator;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Simulator.
 *
 * @return A clone of the Simulator.
 */
//------------------------------------------------------------------------------
GmatBase* Simulator::Clone() const
{
   return new Simulator(*this);
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void  Simulator::Copy(const GmatBase* orig)
{
   operator=(*((Simulator*)(orig)));
}


//------------------------------------------------------------------------------
//  void WriteToTextFile(SolverState stateToUse)
//------------------------------------------------------------------------------
/**
 * Writes state data to the simulator text file.
 *
 * @param stateToUse  sate to use for writing the information to the text file.
 */
//------------------------------------------------------------------------------
void Simulator::WriteToTextFile(SolverState stateToUse)
{
   #ifdef DEBUG_SIMULATOR_WRITE
   MessageInterface::ShowMessage
      ("Sim::WriteToTextFile() entered, stateToUse=%d, solverTextFile='%s', "
       "textFileOpen=%d, initialized=%d\n", stateToUse, solverTextFile.c_str(),
       textFile.is_open(), initialized);
   #endif

   if (!showProgress)
      return;

   if (!textFile.is_open())
      OpenSolverTextFile();

   StringArray::iterator current;

   if (isInitialized)
   {
      switch (currentState)
      {
      case INITIALIZING:
         // This state is basically a "paused state" used for the Target
         // command to finalize the initial data for the variables and
         // goals.  All that is written here is the header information.
         {
            textFile << "************************************************"
                     << "********\n"
                     << "*** Performing Simulation "
                     << "(using \"" << instanceName << "\")\n";

            // Write out the setup data
            textFile << "*** " ;

            // Iterate through TBD, writing them to
            // the file
//               for (current = variableNames.begin(), i = 0;
//                    current != variableNames.end(); ++current)
//               {
//                  if (current != variableNames.begin())
//                     progress << ", ";
//                  progress << *current;
//               }

            textFile << "\n****************************"
                     << "****************************";
         }
         break;

      case PROPAGATING:
         textFile << "\n";
         break;

      case CALCULATING:
         textFile << "\n";
         // Iterate through the TBD variables, writing them to the string
//            for (current = variableNames.begin(), i = 0;
//                 current != variableNames.end(); ++current)
//            {
//               if (current != variableNames.begin())
//                  progress << ", ";
//               progress << *current << " = " << variable.at(i++);
//            }
         break;

      case SIMULATING:
         // TBD
         textFile << "\n";

         break;

      case FINISHED:
         // TBD
         textFile << "\n";

         break;

      default:
         throw SolverException(
            "Solver state not supported for the simulator");
      }
   }
}


//------------------------------------------------------------------------------
//  Real GetTimeStep()
//------------------------------------------------------------------------------
/**
 * Returns the time step of the simulator.
 */
//------------------------------------------------------------------------------
Real Simulator::GetTimeStep()
{
   return timeStep;
}


//------------------------------------------------------------------------------
//  Real GetPropagator()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the PropSetup object.
 */
//------------------------------------------------------------------------------
PropSetup* Simulator::GetPropagator()
{
   return propagator;
}


//------------------------------------------------------------------------------
//  MeasurementManager* GetMeasurementManager()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the MeasurmentMabager object.
 */
//------------------------------------------------------------------------------
MeasurementManager*  Simulator::GetMeasurementManager()
{
   return &measManager;
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id ID for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Simulator::GetParameterText(const Integer id) const
{
   if (id >= SolverParamCount && id < SimulatorParamCount)
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
std::string Simulator::GetParameterUnit(const Integer id) const
{
   return Solver::GetParameterUnit(id); // TBD
}

//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str String for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Simulator::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverParamCount; i < SimulatorParamCount; i++)
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
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Simulator::GetParameterType(const Integer id) const
{
   if (id >= SolverParamCount && id < SimulatorParamCount)
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
 */
//------------------------------------------------------------------------------
std::string Simulator::GetParameterTypeString(const Integer id) const
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
 */
//------------------------------------------------------------------------------
Real Simulator::GetRealParameter(const Integer id) const
{
   if (id == MEASUREMENT_TIME_STEP)
      return simulationStep;

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
Real Simulator::SetRealParameter(const Integer id, const Real value)
{
   if (id == MEASUREMENT_TIME_STEP)
   {
      simulationStep = value;
      return true;
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
std::string Simulator::GetStringParameter(const Integer id) const
{
   if (id == PROPAGATOR)             return propagatorName;
   if (id == INITIAL_EPOCH_FORMAT)   return initialEpochFormat;
   if (id == INITIAL_EPOCH)          return initialEpoch;
   if (id == FINAL_EPOCH_FORMAT)     return finalEpochFormat;
   if (id == FINAL_EPOCH)            return finalEpoch;

   return Solver::GetStringParameter(id);
}


//-----------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//-----------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a StringArray
 *
 * @param id The ID for the parameter
 * @param index The index into the array
 *
 * @return The string
 */
//-----------------------------------------------------------------------------
std::string Simulator::GetStringParameter(const Integer id,
                                          const Integer index) const
{
   if (id == MEASUREMENTS)
   {
      StringArray measList = measManager.GetMeasurementNames();
      if (index < 0 || (index >= (Integer) (measList.size())))
      {
         std::string errmsg = "Simulator::GetStringParameter - Index into "
                  "measurement names ";
         errmsg += instanceName + " is out of range.\n";
         throw SolverException(errmsg);
      }
      return measList.at(index);
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
 * @exception <EstimatorException> thrown if value is out of range
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool Simulator::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_SIMULATOR_INITIALIZATION
      MessageInterface::ShowMessage(
               "Simulator::SetStringParameter(%d, %s)\n",
            id, value.c_str());
   #endif

   if (id == MEASUREMENTS)
   {
      Integer sz = (Integer) measList.size();
      return SetStringParameter(id, value, sz);
   }

   if (id == PROPAGATOR)
   {
      propagatorName = value;  // get propSetup here???
      return true;
   }
   if (id == INITIAL_EPOCH_FORMAT)
   {
      initialEpochFormat = value;
      return true;
   }
   if (id == INITIAL_EPOCH)
   {
      initialEpoch = value;
      // Convert to a.1 time for internal processing
      simulationStart = ConvertToRealEpoch(initialEpoch, initialEpochFormat);
      return true;
   }
   if (id == FINAL_EPOCH_FORMAT)
   {
      finalEpochFormat = value;
      return true;
   }
   if (id == FINAL_EPOCH)
   {
      finalEpoch = value;
      // Convert to a.1 time for internal processing
      simulationEnd = ConvertToRealEpoch(finalEpoch, finalEpochFormat);
      return true;
   }

   return Solver::SetStringParameter(id, value);
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
 * @return
 */
//------------------------------------------------------------------------------
Real Simulator::ConvertToRealEpoch(const std::string &theEpoch,
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
//  bool  SetStringParameter(const Integer id, const std::string &value,
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
 */
//------------------------------------------------------------------------------
bool Simulator::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{
   #ifdef DEBUG_SIMULATOR_INITIALIZATION
      MessageInterface::ShowMessage(
               "Simulator::SetStringParameter(%d, %s, %d)\n",
            id, value.c_str(), index);
   #endif
   if (id == MEASUREMENTS)
   {
      Integer sz = (Integer) measList.size();
      if (index == sz) // needs to be added to the end of the list
      {
         measList.push_back(value);
         measManager.AddMeasurementName(value);
      }
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Simulator::SetStringParameter error - index "
                  "into measurement ";
         errmsg += "array is out of bounds.\n";
         throw SolverException(errmsg);
      }
      else // is in bounds
      {
         measList.at(index) = value;
         measManager.AddMeasurementName(value);
      }
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
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& Simulator::GetStringArrayParameter(const Integer id) const
{
   if (id == MEASUREMENTS)
   {
      return measList; // temporary
   }
   return Solver::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the list of allowable settings for the enumerated parameters
 *
 * @param id The ID of the parameter
 *
 * @return A const string array with the allowed settings.
 */
//------------------------------------------------------------------------------
const StringArray& Simulator::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   enumStrings.clear();

   // todo This list should come from TimeSystemConverter in the util folder
   if ((id == INITIAL_EPOCH_FORMAT) || (id == FINAL_EPOCH_FORMAT))
   {
      enumStrings.push_back("A1ModJulian");
      enumStrings.push_back("TAIModJulian");
      enumStrings.push_back("UTCModJulian");
      enumStrings.push_back("TTModJulian");
      enumStrings.push_back("A1Gregorian");
      enumStrings.push_back("TAIGregorian");
      enumStrings.push_back("UTCGregorian");
      enumStrings.push_back("TTGregorian");
      return enumStrings;
   }

   return Solver::GetPropertyEnumStrings(id);
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
Gmat::ObjectType Simulator::GetPropertyObjectType(const Integer id) const
{
   // The change below breaks sample missions, so I'm backing it out.  A
   // different change committed today fixes the issue.
//   if (id == MEASUREMENTS)
////      return Gmat::MEASUREMENT_MODEL;			// made change by TUAN NGUYEN to pass interpreter validation 4/2/2013
//      return Gmat::TRACKING_SYSTEM;				// made change by TUAN NGUYEN to pass interpreter validation 4/2/2013

   if (id == MEASUREMENTS)
      return Gmat::MEASUREMENT_MODEL;         // made change by TUAN NGUYEN to pass interpreter validation 4/2/2013

   if (id == PROPAGATOR)
      return Gmat::PROP_SETUP;

   return Solver::GetPropertyObjectType(id);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type,
//       const std::string & oldName, const std::string & newName)
//------------------------------------------------------------------------------
/**
 * Renames reference objects
 *
 * @param type The type of object that is renamed
 * @param oldName The name of the object that is changing
 * @param newName the new object name
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Simulator::RenameRefObject(const Gmat::ObjectType type,
      const std::string & oldName, const std::string & newName)
{
   /// @todo Simulator rename code needs to be implemented
   return Solver::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Renames a reference object
 *
 * @param type The object's type
 * @param name The name of the object
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Simulator::SetRefObjectName(const Gmat::ObjectType type,
      const std::string & name)
{
   return Solver::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray & GetRefObjectTypeArray()
//-----------------------------------------------------------------------------
/**
 * Retrieves the list of reference object types in the class
 *
 * @return The reference object type list
 */
//-----------------------------------------------------------------------------
const ObjectTypeArray & Simulator::GetRefObjectTypeArray()
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
const StringArray& Simulator::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   #ifdef DEBUG_SIMULATOR_INITIALIZATION
      MessageInterface::ShowMessage(
            "Simulator::GetRefObjectNameArray(%d) entered\n", type);
   #endif

   refObjectList.clear();

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::PROP_SETUP) ||
       (type == Gmat::MEASUREMENT_MODEL))
   {
      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::PROP_SETUP))
      {
         #ifdef DEBUG_SIMULATOR_INITIALIZATION
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
         for (StringArray::iterator i = measList.begin();
               i != measList.end(); ++i)
         {
            #ifdef DEBUG_SIMULATOR_INITIALIZATION
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


//-----------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//-----------------------------------------------------------------------------
/**
 * Retrieves the name of a reference object
 *
 * @param type the type of the object
 *
 * @return The object's name
 */
//-----------------------------------------------------------------------------
std::string Simulator::GetRefObjectName(const Gmat::ObjectType type) const
{
   return Solver::GetRefObjectName(type);
}


//-----------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string & name)
//-----------------------------------------------------------------------------
/**
 * Retrieves a referenced object
 *
 * @param type The type of the object
 * @param the object's name
 *
 * @return The object
 */
//-----------------------------------------------------------------------------
GmatBase* Simulator::GetRefObject(const Gmat::ObjectType type,
      const std::string & name)
{
   return Solver::GetRefObject(type, name);
}

//-----------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string & name,
//       const Integer index)
//-----------------------------------------------------------------------------
/**
 * Retrieves a referenced object from an array of objects
 *
 * @param type The type of the object
 * @param the object's name
 * @param index The index of the object in the array
 *
 * @return The object
 */
//-----------------------------------------------------------------------------
GmatBase* Simulator::GetRefObject(const Gmat::ObjectType type,
      const std::string & name, const Integer index)
{
   return Solver::GetRefObject(type, name, index);
}


//-----------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string & name)
//-----------------------------------------------------------------------------
/**
 * Sets the pointer to a referenced object on the Simulator
 *
 * @param obj The object
 * @param type The type of the object
 * @param name The object's name
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool Simulator::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name)
{
   #ifdef DEBUG_SIMULATOR_INITIALIZATION
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
      if (obj->IsOfType(Gmat::MEASUREMENT_MODEL) &&
          !(obj->IsOfType(Gmat::TRACKING_SYSTEM)))
      {
         measManager.AddMeasurement((MeasurementModel *)obj);
         return true;
      }
      if (obj->IsOfType(Gmat::TRACKING_SYSTEM))
      {
         #ifdef DEBUG_SIMULATOR_INITIALIZATION
            MessageInterface::ShowMessage("Loading the measurement manager\n");
         #endif

         MeasurementModel *meas;
         // Retrieve each measurement model from the tracking system ...
         for (UnsignedInt i = 0;
                  i < ((TrackingSystem*)obj)->GetMeasurementCount(); ++i)
         {
            #ifdef DEBUG_SIMULATOR_INITIALIZATION
               MessageInterface::ShowMessage("   Measurement %d\n", i);
            #endif

            // ...and pass them to the measurement manager
            meas = ((TrackingSystem*)obj)->GetMeasurement(i);
            if (meas == NULL)
            {
               MessageInterface::ShowMessage("Simulator cannot initialize "
                        "because an expected MeasurementModel is NULL\n");
               throw EstimatorException("In Simulator::SetRefObject, a "
                        "measurement in the tracking system " + obj->GetName() +
                        " is NULL\n");
            }

            measManager.AddMeasurement(meas);
         }
         return true;
      }
   }

   return Solver::SetRefObject(obj, type, name);
}


//-----------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string & typeString)
//-----------------------------------------------------------------------------
/**
 * Retrieves an array of referenced objects
 *
 * @param typeString The text string describing the object
 *
 * @return The array
 */
//-----------------------------------------------------------------------------
ObjectArray& Simulator::GetRefObjectArray(const std::string & typeString)
{
   return Solver::GetRefObjectArray(typeString);
}


//-----------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string & name, const Integer index)
//-----------------------------------------------------------------------------
/**
 * Sets a reference object in a referenced object array
 *
 * @param obj The object
 * @param type The type of the object
 * @param name The object's name
 * @param index The index into the array
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool Simulator::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name, const Integer index)
{
   #ifdef DEBUG_SIMULATOR_INITIALIZATION
      MessageInterface::ShowMessage(""
            "Setting indexed ref object %s with type %s\n", name.c_str(),
            obj->GetTypeName().c_str());
   #endif

   return Solver::SetRefObject(obj, type, name, index);
}

//-----------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//-----------------------------------------------------------------------------
/**
 * Retrieves an array of referenced objects
 *
 * @param type The type of the object
 *
 * @return The array
 */
//-----------------------------------------------------------------------------
ObjectArray& Simulator::GetRefObjectArray(const Gmat::ObjectType type)
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
//  bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * This method performs an action on the instance.
 *
 * TakeAction is a method overridden from GmatBase.  The only action defined for
 * a Simulator is "Reset" which resets the state to INITIALIZING
 *
 * @param action      Text label for the action.
 * @param actionData  Related action data, if needed.
 *
 * @return  flag indicating successful completion or not.
 */
//------------------------------------------------------------------------------
bool Simulator::TakeAction(const std::string &action,
                           const std::string &actionData)
{
   // @todo  Complete Reset action (?) and add others if needed
   if (action == "Reset")
   {
      currentState = INITIALIZING;
      isInitialized = false;
      return true;
   }

   return Solver::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the simulator - checks for unset references and does some
 * validation checking.
 */
//------------------------------------------------------------------------------
bool Simulator::Initialize()
{
   // check the validity of the input start and end times
   if (simulationEnd < simulationStart)
      throw SolverException(
            "Simulator error - simulation end time is before simulation start time.\n");
   // Check to make sure required objects have been set
   if (!propagator)
      throw SolverException(
            "Simulator error - no propagator set for simulator object.\n");
//   if (measList.empty())
//      throw SolverException("Simulator error - no measurements set.\n");

   return true;
}


//------------------------------------------------------------------------------
//  Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * Advances the simulator to the next state.
 */
//------------------------------------------------------------------------------
Solver::SolverState Simulator::AdvanceState()
{
   switch (currentState)
   {
      case INITIALIZING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Simulator state machine: "
                     "INITIALIZING\n");
         #endif
         // ReportProgress() writes nonsense here, so removed for now
         // ReportProgress();
         CompleteInitialization();
         break;

      case PROPAGATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Simulator state machine: "
                     "PROPAGATING\n");
         #endif
         // ReportProgress() writes blank lines here, so removed for now
         // ReportProgress();
         FindTimeStep();
         break;

      case CALCULATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Simulator state machine: "
                     "CALCULATING\n");
         #endif
         // ReportProgress() writes blank lines here, so removed for now
         // ReportProgress();
         CalculateData();
         break;

      case LOCATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Simulator state machine: "
                     "LOCATING\n");
         #endif
         //ReportProgress();
         ProcessEvent();
         break;

      case SIMULATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Simulator state machine: "
                     "SIMULATING\n");
         #endif
         // ReportProgress() writes blank lines here, so removed for now
         // ReportProgress();
         SimulateData();
         break;

      case FINISHED:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Simulator state machine: "
                     "FINISHED\n");
         #endif
         RunComplete();
         // ReportProgress() writes blank lines here, so removed for now
         // ReportProgress();
         break;

      default:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered Simulator state machine: "
               "Bad state for a simulator.\n");
         #endif
         /* throw EstimatorException("Solver state not supported for the simulator")*/;
   }

   return currentState;

}


//------------------------------------------------------------------------------
//  bool Finalize()
//------------------------------------------------------------------------------
/**
 * Finalizes the simulator.
 */
//------------------------------------------------------------------------------
bool Simulator::Finalize()
{
   return true;
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
bool Simulator::HasLocalClones()
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
void Simulator::UpdateClonedObject(GmatBase *obj)
{
   throw SolverException("To do: implement Simulator::UpdateClonedObject");
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * This method completes initialization for the Simulator object, initializing
 * its MeasurementManager, retrieving the epoch and setting the state.
 *
 */
//------------------------------------------------------------------------------
void Simulator::CompleteInitialization()
{
   // tell the measManager to complete its initialization
   bool measOK = measManager.Initialize();
   if (!measOK)
      throw SolverException(
            "Simulator::CompleteInitialization - error initializing "
            "MeasurementManager.\n");

   nextSimulationEpoch = simulationStart;
   timeStep            = (nextSimulationEpoch - currentEpoch) *
                          GmatTimeConstants::SECS_PER_DAY;

   if (GmatMathUtil::IsEqual(currentEpoch, nextSimulationEpoch,
            SIMTIME_ROUNDOFF))
      currentState = CALCULATING;
   else
      currentState = PROPAGATING;

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Epoch is %.12lf, Start epoch is %.12lf\n",
               currentEpoch, nextSimulationEpoch);

      MessageInterface::ShowMessage("State is %s\n",
               (currentState == CALCULATING ? "Calculating" : "Propagating"));
   #endif

   isInitialized = true;
}


//------------------------------------------------------------------------------
//  void FindTimeStep()
//------------------------------------------------------------------------------
/**
 * This method determines whether the simulation is finished or still
 * calculating, and if neither, computes the timeStep.
 */
//------------------------------------------------------------------------------
void Simulator::FindTimeStep()
{
   if (currentEpoch > simulationEnd)
   {
      currentState = FINISHED;
   }
   else if (GmatMathUtil::IsEqual(currentEpoch, nextSimulationEpoch,
            SIMTIME_ROUNDOFF))
   {
      currentState = CALCULATING;
   }
   else
   {
      // Calculate the time step in seconds and stay in the PROPAGATING state;
      // timeStep could be positive or negative, and is good to 1 microsec
      timeStep = ((Integer)((nextSimulationEpoch - currentEpoch) *
               GmatTimeConstants::SECS_PER_DAY * 1000000))/1000000.0;

      #ifdef DEBUG_TIMESTEP
         MessageInterface::ShowMessage("Simulator time step = %.12lf based on "
                  "current epoch = %.12lf and next epoch = %.12lf\n", timeStep,
                  currentEpoch, nextSimulationEpoch);
      #endif

      #ifdef DEBUG_SIMULATION
         MessageInterface::ShowMessage("Timestep = %.14lf\n", timeStep);
      #endif
   }
}


//------------------------------------------------------------------------------
//  void CalculateData()
//------------------------------------------------------------------------------
/**
 * This method determines whether or not measurements are possible, and advances
 * the state.
 */
//------------------------------------------------------------------------------
void Simulator::CalculateData()
{
   // Tell the measurement manager to calculate the simulation data
   if (measManager.CalculateMeasurements() == false)
   {
      // No measurements were possible
      FindNextSimulationEpoch();

      if ((currentEpoch < simulationEnd) &&
          (nextSimulationEpoch < simulationEnd))
         currentState = PROPAGATING;
      else
         currentState = FINISHED;
   }
   else
   {
      // Measurements are possible!
      if (measManager.MeasurementHasEvents())
      {
         currentState = LOCATING;
         locatingEvent = true;
      }
      else
         currentState = SIMULATING;
   }
}


//------------------------------------------------------------------------------
//  void ProcessEvent()
//------------------------------------------------------------------------------
/**
 * This method manages the state machine operations while processing events.
 */
//------------------------------------------------------------------------------
 void Simulator::ProcessEvent()
{
   locatingEvent = false;

   for (UnsignedInt i = 0; i < activeEvents.size(); ++i)
   {
      #ifdef DEBUG_EVENT
            MessageInterface::ShowMessage("*** Checking event %d\n", i);
      #endif
      if (((Event*)activeEvents[i])->CheckStatus() != LOCATED)
      {
         locatingEvent = true;
         #ifdef DEBUG_EVENT
            MessageInterface::ShowMessage("   *** %d not yet located\n", i);
         #endif
      }
      else
      {
         if (measManager.ProcessEvent((Event*)activeEvents[i]) == false)
            MessageInterface::ShowMessage("Event %d located but not "
                  "processed!\n", i);
         #ifdef DEBUG_EVENT
            MessageInterface::ShowMessage("   *** %d located!\n", i);
         #endif
      }
   }

   // If all located, move into the SIMULATING state
   if (!locatingEvent)
   {
      currentState = SIMULATING;
   }
}


//------------------------------------------------------------------------------
//  void CalculateData()
//------------------------------------------------------------------------------
/**
 * This method tells its MeasurementManager to add noise and write the data,
 * finds the nextSimulationEpoch, and advances the state.
 */
//------------------------------------------------------------------------------
void Simulator::SimulateData()
{
   // Tell the measurement manager to add noise and write the measurements
   if (measManager.CalculateMeasurements(true) == true)
   {
      if (measManager.WriteMeasurements() == false)
         /*throw EstimatorException("Measurement writing failed")*/;
   }

   // Prep for the next measurement simulation
   FindNextSimulationEpoch();

   if ((currentEpoch < simulationEnd) && (nextSimulationEpoch < simulationEnd))
      currentState = PROPAGATING;
   else
      currentState = FINISHED;
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * This method updates the simulator text file at the end of a simulator run.
 */
//------------------------------------------------------------------------------
void Simulator::RunComplete()
{
   WriteToTextFile();
   // tell the MeasurementManager to close files and finalize
   measManager.Finalize();
}


//------------------------------------------------------------------------------
//  void FindNextSimulationEpoch()
//------------------------------------------------------------------------------
/**
 * This method computes the nextSimulationEpoch.
 *
 * @note This might become more complicated down the road.
 */
//------------------------------------------------------------------------------
void Simulator::FindNextSimulationEpoch()
{
   // we are assuming that the simulationStep is always non-negative
   nextSimulationEpoch = currentEpoch +
         simulationStep / GmatTimeConstants::SECS_PER_DAY;

   #ifdef DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Current epoch = %.12lf; "
            "next sim epoch = %.12lf, sim end = %.12lf\n", currentEpoch,
            nextSimulationEpoch, simulationEnd);
   #endif
}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string for reporting the current simulator state.
 */
//------------------------------------------------------------------------------
std::string Simulator::GetProgressString()
{
   StringArray::iterator current;
//   Integer i;
   std::stringstream progress;
   progress.str("");
   progress.precision(12);

   if (isInitialized)
   {
      switch (currentState)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Target
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               progress << "************************************************"
                        << "********\n"
                        << "*** Performing Simulation "
                        << "(using \"" << instanceName << "\")\n";

               // Write out the setup data
               progress << "*** " ;

               // Iterate through TBD, writing them to
               // the file
//               for (current = variableNames.begin(), i = 0;
//                    current != variableNames.end(); ++current)
//               {
//                  if (current != variableNames.begin())
//                     progress << ", ";
//                  progress << *current;
//               }

               progress << "\n****************************"
                        << "****************************";
            }
            break;

         case PROPAGATING:
//            progress << "\n";
            break;

         case CALCULATING:
//            progress << "\n";
            // Iterate through the TBD variables, writing them to the string
//            for (current = variableNames.begin(), i = 0;
//                 current != variableNames.end(); ++current)
//            {
//               if (current != variableNames.begin())
//                  progress << ", ";
//               progress << *current << " = " << variable.at(i++);
//            }
            break;

         case LOCATING:
            // TBD
//            progress << "\n";

            break;

         case SIMULATING:
            // TBD
//            progress << "\n";

            break;

         case FINISHED:
            // TBD
//            progress << "\n";

            break;

         default:
            throw SolverException(
               "Solver state not supported for the simulator");
      }
   }
   else
      return Solver::GetProgressString();

   return progress.str();
}


//-----------------------------------------------------------------------------
// void UpdateCurrentEpoch(GmatEpoch newEpoch)
//-----------------------------------------------------------------------------
/**
 * Sets the current epoch to a new value
 *
 * @param newEpoch The new epoch
 */
//-----------------------------------------------------------------------------
void Simulator::UpdateCurrentEpoch(GmatEpoch newEpoch)
{
   currentEpoch = newEpoch;
}

//------------------------------------------------------------------------------
// unused methods
//------------------------------------------------------------------------------

// Methods required by base classes

//------------------------------------------------------------------------------
// Integer SetSolverResults(Real*, const std::string&, const std::string&)
//------------------------------------------------------------------------------
/**
 * Registers events for event processing in the Simulator
 *
 * @param
 * @param
 * @param
 *
 * @return The ID for the event
 */
//------------------------------------------------------------------------------
Integer Simulator::SetSolverResults(Real*, const std::string&,
         const std::string&)
{
   return -1;
}


//------------------------------------------------------------------------------
// void Simulator::SetResultValue(Integer, Real, const std::string&)
//------------------------------------------------------------------------------
/**
 * Passes in event function data during event processing
 *
 * @param
 * @param
 * @param
 */
//------------------------------------------------------------------------------
void Simulator::SetResultValue(Integer eventState, Real val,
      const std::string& eventName)
{
   if (eventState == LOCATED)
      locatingEvent = false;
   if (eventState == SEEKING)
      locatingEvent = true;
}
