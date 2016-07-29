//$Id: Simulator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         Simulator
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
#include "StringUtil.hpp"
#include <sstream>

//#define DEBUG_STATE_MACHINE
//#define DEBUG_SIMULATOR_WRITE
//#define DEBUG_SIMULATOR_INITIALIZATION
//#define DEBUG_INITIALIZATION
//#define DEBUG_TIMESTEP
//#define DEBUG_EVENT
//#define DEBUG_INITIALIZE
//#define DEBUG_CLONED_PARAMETER_UPDATES

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
Simulator::PARAMETER_TEXT[SimulatorParamCount -SolverParamCount] =
{
   "AddData",
   "Propagator",
   "EpochFormat",
   "InitialEpoch",
   "FinalEpoch",
   "MeasurementTimeStep",
   "AddNoise",
};

const Gmat::ParameterType
Simulator::PARAMETER_TYPE[SimulatorParamCount - SolverParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::ON_OFF_TYPE,
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
//   simulationStart     (GmatTimeConstants::MJD_OF_J2000),
//   simulationEnd       (GmatTimeConstants::MJD_OF_J2000 + 1.0),
//   nextSimulationEpoch (GmatTimeConstants::MJD_OF_J2000),
   simEpochCounter     (0),
//   currentEpoch        (GmatTimeConstants::MJD_OF_J2000),
   epochFormat         ("TAIModJulian"),
   initialEpoch        ("21545"),
   finalEpoch          ("21546"),          // ("21545"),  It has 1 day time interval to simulate data
   simulationStep      (60.0),
   locatingEvent       (false),
   timeStep            (60.0),
   addNoise            (false),
   isEpochFormatSet    (false)
{
   objectTypeNames.push_back("Simulator");
   parameterCount = SimulatorParamCount;

   simulationStart = TimeConverterUtil::ConvertFromTaiMjd(TimeConverterUtil::A1MJD, atof(initialEpoch.c_str()));
   simulationEnd = TimeConverterUtil::ConvertFromTaiMjd(TimeConverterUtil::A1MJD, atof(finalEpoch.c_str()));
   currentEpoch = nextSimulationEpoch = simulationStart;

   // Turn off writting progress report for simulation    // fix bug GMT-5713 RunSimulator command creates an empty file
   showProgress = false;                                  // fix bug GMT-5713 RunSimulator command creates an empty file
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
   simEpochCounter     (0),
   currentEpoch        (sim.currentEpoch),
   epochFormat         (sim.epochFormat),
   initialEpoch        (sim.initialEpoch),
   finalEpoch          (sim.finalEpoch),
   simulationStep      (sim.simulationStep),
   locatingEvent       (false),
   timeStep            (sim.timeStep),
   measManager         (sim.measManager),
   measList            (sim.measList),
   addNoise            (sim.addNoise),
   isEpochFormatSet    (sim.isEpochFormatSet)
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

      if (sim.propagator) 
         propagator = ((PropSetup*) (sim.propagator)->Clone());
      else
         propagator = NULL;

      propagatorName      = sim.propagatorName;
      simState            = NULL;   // or clone it here??
      simulationStart     = sim.simulationStart;
      simulationEnd       = sim.simulationEnd;
      nextSimulationEpoch = sim.nextSimulationEpoch;
      simEpochCounter     = sim.simEpochCounter;
      currentEpoch        = sim.currentEpoch;
      epochFormat         = sim.epochFormat;
      initialEpoch        = sim.initialEpoch;
      finalEpoch          = sim.finalEpoch;
      simulationStep      = sim.simulationStep;
      timeStep            = sim.timeStep;
      locatingEvent       = false;
      measManager         = sim.measManager;
      measList            = sim.measList;
      addNoise            = sim.addNoise;
      isEpochFormatSet    = sim.isEpochFormatSet;
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

   if (simState)
      delete simState;

   activeEvents.clear();
   measList.clear();
   measModelList.clear();
   refObjectList.clear();
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
       "textFileOpen=%d, isInitialized=%d\n", stateToUse, solverTextFile.c_str(),
       textFile.is_open(), isInitialized);
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
Real Simulator::GetTimeStep(GmatEpoch fromEpoch)
{
   if (fromEpoch > 0.0)
      timeStep = (nextSimulationEpoch - fromEpoch) *
                             GmatTimeConstants::SECS_PER_DAY;

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
   std::string str1 = str;


   // This section is used to throw an exception for unused parameters
   if ((str1 == "ShowProgress")||(str1 == "ReportFile")||(str1 == "ReportStyle")||(str1 == "MaximumIterations"))
      throw SolverException("Syntax error: simulator '" + GetName() + "' does not has parameter '" + str1 + "'.\n");

   for (Integer i = SolverParamCount; i < SimulatorParamCount; i++)
   {
      if (str1 == PARAMETER_TEXT[i - SolverParamCount])
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
//  bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method is used to specify a parameter is read only or not.
 *
 * @param id ID for the requested parameter.
 *
 * @return   true if parameter read only, false otherwise.
 */
//------------------------------------------------------------------------------
bool Simulator::IsParameterReadOnly(const Integer id) const
{
   if ((id == ShowProgressID)||(id == ReportStyle)||
      (id == solverTextFileID)||(id == maxIterationsID))
      return true;

   return Solver::IsParameterReadOnly(id);
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
 * @param id         ID for the parameter whose value to change.
 * @param value      Value for the parameter (unit: second).
 *
 * @return  Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Simulator::SetRealParameter(const Integer id, const Real value)
{
   if (id == MEASUREMENT_TIME_STEP)
   {
      #ifdef DEBUG_TIMESTEP
        MessageInterface::ShowMessage("simulationStep = %.15lf\n", value);
      #endif
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "Error: a nonpositive number (" << value << ") was set to " << GetName() << ".MeasurementTimeStep parameter. It should be a positive number.\n";
         throw SolverException(ss.str());
      }

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
   if (id == EPOCH_FORMAT)           return epochFormat;
   if (id == INITIAL_EPOCH)          return initialEpoch;
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
 * @exception <SolverException> thrown if value is out of range
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool Simulator::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_SIMULATOR_INITIALIZATION
      MessageInterface::ShowMessage(
               "Simulator<%s,%p>::SetStringParameter(%d, %s)\n",GetName().c_str(), this, 
            id, value.c_str());
   #endif

   if (id == MEASUREMENTS)
   {
      std::string measName = GmatStringUtil::Trim(GmatStringUtil::RemoveOuterString(value, "{", "}"));
      if (measName == "")
         throw SolverException("Error: No measurement or tracking file set is set to " + GetName() + ".AddData parameter.\n");

      // verify a valid object name
      if (!GmatStringUtil::IsValidIdentity(value))
         throw SolverException("Error: An invalid value '" + value + "' is set to " + GetName() + ".AddData parameter.\n");

      return SetStringParameter(id, value, measList.size());
   }

   if (id == PROPAGATOR)
   {
      if (!GmatStringUtil::IsValidIdentity(value))
         throw SolverException("Error: '" + value + "' set to " + GetName() + ".Propagator parameter is an invalid object name.\n");

      propagatorName = value;  // get propSetup here???   Answer: GMAT cannot get propSetup object here due to at this point, the script to define propSetup object may be not read yet. Getting propSetup should be in SetRefObject()
      return true;
   }
   if (id == EPOCH_FORMAT)
   {
      if (!TimeConverterUtil::IsValidTimeSystem(value))
         throw SolverException("Error: Time system '" + value + "' set to " + GetName() + ".EpochFormat parameter is invalid.\n");

      epochFormat = value;
      isEpochFormatSet = true;
      return true;
   }
   if (id == INITIAL_EPOCH)
   {
      if (!isEpochFormatSet)
         MessageInterface::ShowMessage("Warning: In your script, %s.EpochFormat parameter has to be set before setting %s.InitialEpoch.\n", GetName().c_str(), GetName().c_str());

      initialEpoch = value;
      // Convert to a.1 time for internal processing
      simulationStart = ConvertToRealEpoch(initialEpoch, epochFormat);
      return true;
   }
   if (id == FINAL_EPOCH)
   {
      if (!isEpochFormatSet)
         MessageInterface::ShowMessage("Warning: In your script, %s.EpochFormat parameter has to be set before setting %s.FinalEpoch.\n", GetName().c_str(), GetName().c_str());

      finalEpoch = value;
      // Convert to a.1 time for internal processing
      simulationEnd = ConvertToRealEpoch(finalEpoch, epochFormat);
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
 * @exception <SolverException> thrown if value is out of range
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
      // No measurement is added when an empty list is set to Add Measurement parameter 
      if (index == -1)
         return true;

      if (!GmatStringUtil::IsValidIdentity(value))
         throw SolverException("Error: '" + value + "' set to " + GetName() + ".Measurements parameter is an invalid object name.\n");

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
// std::string GetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns On/Off parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  On/Off value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Simulator::GetOnOffParameter(const Integer id) const
{
   if (id == ADD_NOISE)
      return (addNoise ? "On" : "Off");

   return Solver::GetOnOffParameter(id);
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets On/Off value to a parameter, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param value   On/Off value used to set to the parameter
 *
 * @return  true if value id On, false otherwise.
 *
 */
//------------------------------------------------------------------------------
bool Simulator::SetOnOffParameter(const Integer id, const std::string &value)
{
   if (id == ADD_NOISE)
   {
      if (value == "On")
      {
         addNoise = true;
         return true;
      } 
      else if (value == "Off")
      {
         addNoise = false;
         return true;
      }

      return false;
   }

   return Solver::SetOnOffParameter(id, value);
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
   static StringArray typeList;
   typeList.clear();

   if (id == EPOCH_FORMAT)
   {
      typeList = TimeConverterUtil::GetValidTimeRepresentations();
      return typeList;
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
////      return Gmat::MEASUREMENT_MODEL;         // made change to pass interpreter validation 4/2/2013
//      return Gmat::TRACKING_SYSTEM;            // made change to pass interpreter validation 4/2/2013

   if (id == MEASUREMENTS)
      return Gmat::MEASUREMENT_MODEL;         // made change to pass interpreter validation 4/2/2013

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
   if (type == Gmat::PROP_SETUP)
   {
      if (propagator->GetName() == oldName)
      {
         propagator->SetName(newName);
         return true;
      }
   }

   if (type == Gmat::MEASUREMENT_MODEL)
   {
      for(UnsignedInt i = 0; i < measModelList.size(); ++i)
      {
         if (measModelList[i]->GetName() == oldName)
         {
            measModelList[i]->SetName(newName);
            return true;
         }
      }
   }

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
   if (type == Gmat::PROP_SETUP)
   {
      propagator->SetName(name);
      return true;
   }
   // Note: this function is not applied for measurement List

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
   static ObjectTypeArray objTypes = Solver::GetRefObjectTypeArray();
   objTypes.push_back(Gmat::PROP_SETUP);
   objTypes.push_back(Gmat::MEASUREMENT_MODEL);
   objTypes.push_back(Gmat::DATA_FILTER);
//   objTypes.push_back(Gmat::TRACKING_SYSTEM);
   return objTypes;

   //return Solver::GetRefObjectTypeArray();
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
   refObjectList = Solver::GetRefObjectNameArray(type);

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
   if (type == Gmat::PROP_SETUP)
      return propagator->GetName();

   // Note: this function is not applied for measurement List

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
   if (type == Gmat::PROP_SETUP)
   {
      if (propagator != NULL)
      {
         if (propagator->GetName() == name)
            return propagator;
      }
   }

   if (type == Gmat::MEASUREMENT_MODEL)
   {
      for (UnsignedInt i = 0; i < measModelList.size(); ++i)
      {
         if (measModelList[i]->GetName() == name)
            return measModelList[i];
      }
   }

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
         #ifdef DEBUG_SIMULATOR_INITIALIZATION
            MessageInterface::ShowMessage("SetRefObject for object type Gmat::MEASUREMENT_MODEL.\n");
         #endif

         measManager.AddMeasurement((MeasurementModel *)obj);
         return true;
      }
      if (obj->IsOfType(Gmat::TRACKING_SYSTEM))
      {
         #ifdef DEBUG_SIMULATOR_INITIALIZATION
            MessageInterface::ShowMessage("SetRefObject <%s> for object type Gmat::TRACKING_SYSTEM.\n", obj->GetName().c_str());
         #endif

         // Add to tracking systems list
         measManager.AddMeasurement((TrackingSystem*)obj);

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
               throw SolverException("In Simulator::SetRefObject, a "
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
   return GetRefObjectArray(GetObjectType(typeString));
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
   #ifdef DEBUG_SIMULATOR_INITIALIZATION
      MessageInterface::ShowMessage("GetRefObjectArray(type = %d)\n", type);
   #endif


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
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Start Simulator::Initialize()\n");
#endif

   // check the validity of the input start and end times
   if (simulationEnd < simulationStart)
      throw SolverException(
            "Simulator error - simulation end time is before simulation start time.\n");

   // Check to make sure required objects have been set
   if (propagatorName == "")
      throw SolverException(
            "Simulator error - " + GetName() + ".Propagator was not defined in your script.\n");

   if (!propagator)
      throw SolverException(
            "Simulator error - Propagator '" + propagatorName + "' was not defined in your script.\n");

   if (measList.empty())
       throw SolverException(
            "Simulator error - " + GetName() + ".AddData was not defined in your script.\n");


   // Check the names of measurement models shown in sim.AddData have to be the names of created objects
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
         throw SolverException("Cannot initialize simulator; '" + name + "' object is not defined in script.\n");
   }


   // Check for TrackingConfig to be defined in TrackingFileSet
   for(UnsignedInt i = 0; i < tfs.size(); ++i)
   {
      StringArray list = tfs[i]->GetStringArrayParameter("AddTrackingConfig");
      if (list.size() == 0)
         throw SolverException("Cannot initialize simulator; TrackingFileSet '" + 
                 tfs[i]->GetName() + "' object which is defined in simulator '" + 
                 GetName() + "' has no tracking configuration.\n");
   }

   measModels.clear();
   tkSystems.clear();
   tfs.clear();
   measNames.clear();


#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Exit Simulator::Initialize()\n");
#endif
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
   #ifdef DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Entered Simulator::AdvanceState()\n");
   #endif
   
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
         /* throw SolverException("Solver state not supported for the simulator")*/;
   }

   #ifdef DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Exit Simulator::AdvanceState()\n");
   #endif
   
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
   if (obj->IsOfType("Spacecraft"))
      return;
   throw SolverException("To do: implement Simulator::UpdateClonedObject "
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
void Simulator::UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
{
#ifdef DEBUG_CLONED_PARAMETER_UPDATES
   MessageInterface::ShowMessage("Simulator updating parameter %d (%s) using "
         "object %s\n", updatedParameterId, obj->GetParameterText(updatedParameterId).c_str(),
         obj->GetName().c_str());
#endif
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

   // Prepare for processing
   measManager.PrepareForProcessing(true);

   // Get time range of EOP file
   EopFile* eop = GmatGlobal::Instance()->GetEopFile();
   if (eop != NULL)
   {
      Real timeMin, timeMax;
      eop->GetTimeRage(timeMin, timeMax);
      if (simulationStart < timeMin)
      {
         MessageInterface::ShowMessage("Warning: %s.InitialEpoch has value (%.12lf A1Mjd) outside EOP time range [%.12lf A1Mjd , %.12lf A1Mjd].\n", GetName().c_str(), simulationStart, timeMin, timeMax);
      }
      else if (simulationEnd > timeMax)
         MessageInterface::ShowMessage("Warning: %s.FinalEpoch has value (%.12lf A1Mjd) outside EOP time range [%.12lf A1Mjd , %.12lf A1Mjd].\n", GetName().c_str(), simulationEnd, timeMin, timeMax);
   }
   else
      MessageInterface::ShowMessage("Warning: No EOP file was used for running this GMAT script.\n");

   if (!measOK)
      throw SolverException(
            "Simulator::CompleteInitialization - error initializing "
            "MeasurementManager.\n");

   // Load ramp table
   measManager.LoadRampTables();

   nextSimulationEpoch = simulationStart;
   simEpochCounter     = 0;
   timeStep            = (nextSimulationEpoch - currentEpoch) *
                          GmatTimeConstants::SECS_PER_DAY;

   if (GmatMathUtil::IsEqual(currentEpoch, nextSimulationEpoch,
            SIMTIME_ROUNDOFF))
      currentState = CALCULATING;
   else
      currentState = PROPAGATING;

   isTheFirstMeasurement = true;                                  // fix bug GMT-4909
   
   
   // Verify no two different ground station having the same Id
   std::string warningMsg = "";
   if (GetMeasurementManager()->ValidateDuplicationOfGroundStationID(warningMsg) == false)
   {
      warningMsg = warningMsg + " in simulator '" + GetName() + "'.\n";
      MessageInterface::ShowMessage("Warning: %s", warningMsg.c_str());
   }
   

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
   if (GmatMathUtil::IsEqual(currentEpoch, nextSimulationEpoch,             // swap order of "if" statements in order to fix bug GMT-5606
      SIMTIME_ROUNDOFF))
   {
      currentState = CALCULATING;
   }
   else if (currentEpoch > simulationEnd)
   {
      if (!isTheFirstMeasurement)                                // fix bug GMT-4909
         currentState = FINISHED;
   }
   else
   {
      // Calculate the time step in seconds and stay in the PROPAGATING state;
      // timeStep could be positive or negative, and is good to 1 microsec
//      timeStep = ((Integer)((nextSimulationEpoch - currentEpoch) *
//               GmatTimeConstants::SECS_PER_DAY * 1000000))/1000000.0;

      timeStep = (nextSimulationEpoch - currentEpoch) *
               GmatTimeConstants::SECS_PER_DAY;               // fixed Bug GMT-3700

      #ifdef DEBUG_TIMESTEP
         MessageInterface::ShowMessage("Simulator time step = %.15lf based on "
                  "current epoch = %.15lf and next epoch = %.15lf\n", timeStep,
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
   //if (measManager.CalculateMeasurements(true, false) == false)               // fixed Bug 8 in ticket GMT-4314
   if (measManager.CalculateMeasurements(true, true, addNoise) == false)        // fixed Bug 8 in ticket GMT-4314
   {
      // No measurements were possible
      FindNextSimulationEpoch();

      //if ((currentEpoch < simulationEnd) &&                                               // fix bug GMT-5606
      //    (nextSimulationEpoch < simulationEnd))                                          // fix bug GMT-5606
      if ((nextSimulationEpoch < simulationEnd) ||                                          // fix bug GMT-5606
          GmatMathUtil::IsEqual(nextSimulationEpoch, simulationEnd, SIMTIME_ROUNDOFF))      // fix bug GMT-5606
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
//   if (measManager.CalculateMeasurements(true, true, addNoise) == true)
//   {

      // Validate media correction for all measurements before writing them to .gmd file
      const MeasurementData* measData = NULL;
      for (Integer i = 0; (measData = measManager.GetMeasurement(i)) != NULL; ++i)
      {
         // Validate media correction for the measurement 
         ValidateMediaCorrection(measData);
      }

      // Write measurements to data file
      if (measManager.WriteMeasurements() == false)
      {
         throw SolverException("Measurement writing failed.\n");
      }

//   }
   
   // Prep for the next measurement simulation
   isTheFirstMeasurement = false;                                    // fix bug GMT-4909
   FindNextSimulationEpoch();
   
//   if ((currentEpoch < simulationEnd) && (nextSimulationEpoch < simulationEnd))        // fix bug GMT-5606
   if ((nextSimulationEpoch < simulationEnd) ||                                          // fix bug GMT-5606
      GmatMathUtil::IsEqual(nextSimulationEpoch, simulationEnd, SIMTIME_ROUNDOFF))       // fix bug GMT-5606
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

   // clear media correction warning lists
   ionoWarningList.clear();
   tropoWarningList.clear();
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
   ++simEpochCounter;
//   nextSimulationEpoch = simulationStart + simEpochCounter *
//         simulationStep / GmatTimeConstants::SECS_PER_DAY;
   nextSimulationEpoch = simulationStart + (simEpochCounter /
         GmatTimeConstants::SECS_PER_DAY) * simulationStep;

   #ifdef DEBUG_TIMESTEP
      MessageInterface::ShowMessage("%d: Start: %.12lf step: %.12lf "
            "Next:%.12lf\n", simEpochCounter, simulationStart, simulationStep,
            nextSimulationEpoch);
   #endif

   #ifdef DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Current epoch = %.15lf; simulationStep = %.15lf;"
            " next sim epoch = %.15lf, sim end = %.15lf\n", currentEpoch, simulationStep, 
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


void Simulator::ValidateMediaCorrection(const MeasurementData* measData)
{
   if (measData->isIonoCorrectWarning)
   {
      // Get measurement pass:
      std::stringstream ss1;
      ss1 << "{{";
      for (Integer i = 0; i < measData->participantIDs.size(); ++i)
      {
         ss1 << measData->participantIDs[i] << (((i + 1) < measData->participantIDs.size()) ? "," : "");
      }
      ss1 << "}," << measData->typeName << "}";

      // if the pass is not in warning list, then display warning message
      if (find(ionoWarningList.begin(), ionoWarningList.end(), ss1.str()) == ionoWarningList.end())
      {
         // generate warning message
         MessageInterface::ShowMessage("Warning: When running simulator '%s', ionosphere correction is %lf m for measurement %s at measurement time tag %.12lf A1Mjd. Media corrections to the computed measurement may be inaccurate.\n", GetName().c_str(), measData->ionoCorrectWarningValue * 1000.0, ss1.str().c_str(), measData->epoch);

         // add pass to the list
         ionoWarningList.push_back(ss1.str());
      }
   }

   if (measData->isTropoCorrectWarning)
   {
      // Get measurement path:
      std::stringstream ss1;
      ss1 << "{{";
      for (Integer i = 0; i < measData->participantIDs.size(); ++i)
      {
         ss1 << measData->participantIDs[i] << (((i + 1) < measData->participantIDs.size()) ? "," : "");
      }
      ss1 << "}," << measData->typeName << "}";

      // if the pass is not in warning list, then display warning message
      if (find(tropoWarningList.begin(), tropoWarningList.end(), ss1.str()) == tropoWarningList.end())
      {
         // generate warning message
         MessageInterface::ShowMessage("Warning: When running simulator '%s', troposphere correction is %lf m for measurement %s at measurement time tag %.12lf A1Mjd. Media corrections to the computed measurement may be inaccurate.\n", GetName().c_str(), measData->tropoCorrectWarningValue * 1000.0, ss1.str().c_str(), measData->epoch);

         // add pass to the list
         tropoWarningList.push_back(ss1.str());
      }
   }
}

