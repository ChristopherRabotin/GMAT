//$Id$
//------------------------------------------------------------------------------
//                                 Propagate
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Implementation for the Propagate command class
 */
//------------------------------------------------------------------------------

#include "Propagate.hpp"
#include "Publisher.hpp"
#include "Parameter.hpp"
#include "StringUtil.hpp" // for Trim()
#include "AngleUtil.hpp"  // for PutAngleInDegRange()
#include "MessageInterface.hpp"

#include <sstream>
#include <cmath>

//#define DEBUG_PROPAGATE_ASSEMBLE 1
//#define DEBUG_PROPAGATE_OBJ 1
//#define DEBUG_PROPAGATE_INIT 1
//#define DEBUG_PROPAGATE_DIRECTION 1
//#define DEBUG_PROPAGATE_STEPSIZE 1
//#define DEBUG_PROPAGATE_EXE 1
//#define DEBUG_STOPPING_CONDITIONS 1
//#define DEBUG_RENAME
//#define DEBUG_PROP_PERFORMANCE
//#define DEBUG_FIRST_CALL
//#define DEBUG_FIXED_STEP
//#define DEBUG_EPOCH_SYNC
//#define DEBUG_SECANT_DETAILS
//#define DEBUG_BISECTION_DETAILS
//#define DEBUG_WRAPPERS

#define TIME_ROUNDOFF 1.0e-6
#define DEFAULT_STOP_TOLERANCE 1.0e-7

//--------------------------------- 
// static data
//---------------------------------
std::string Propagate::PropModeList[PropModeCount] =
{
   "", "Synchronized", "BackProp"
};

const std::string
Propagate::PARAMETER_TEXT[PropagateCommandParamCount - GmatCommandParamCount] =
{
   "AvailablePropModes",
   "PropagateMode",
   "InterruptFrequency",
   "StopTolerance",
   "Spacecraft",
   "Propagator",
   "StopCondition",
   "PropForward"
};

const Gmat::ParameterType
Propagate::PARAMETER_TYPE[PropagateCommandParamCount - GmatCommandParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::BOOLEAN_TYPE
};

#ifdef DUMP_PLANET_DATA
   std::ofstream Propagate::planetData;

   CelestialBody* Propagate::body[11] = 
   {
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL 
   };
   
   Integer Propagate::bodiesDefined = 0;
#endif

#ifdef DEBUG_FIRST_CALL
static bool firstStepFired = false;
#endif

//---------------------------------
// public members
//---------------------------------

//------------------------------------------------------------------------------
//  Propagate()
//------------------------------------------------------------------------------
/**
 * Constructs the Propagate Command (default constructor).
 */
//------------------------------------------------------------------------------
Propagate::Propagate() :
   GmatCommand                 ("Propagate"),
   direction                   (1.0),
   currentPropMode             (""),
   interruptCheckFrequency     (30),
   inProgress                  (false),
   hasFired                    (false),
   epochID                     (-1),
   stopInterval                (0.0),
   stopTrigger                 (-1),
   state                       (NULL),
   pubdata                     (NULL),
   stopCondMet                 (false),
   stopEpoch                   (0.0),
   stopAccuracy                (DEFAULT_STOP_TOLERANCE),
   timeAccuracy                (1.0e-6),
   dim                         (0),
   singleStepMode              (false),
   currentMode                 (INDEPENDENT),
   stopCondEpochID             (-1),
   stopCondBaseEpochID         (-1),
   stopCondStopVarID           (-1)
{
   stepBrackets[0] = stepBrackets[1] = 0.0;
   parameterCount = PropagateCommandParamCount;
   
   // First step tolerance is one order of magnitude above stop accuracy.
   firstStepTolerance = stopAccuracy * 10.0;

   #ifdef DUMP_PLANET_DATA
      if (!planetData.is_open())
      {
         planetData.open("PlanetaryEphem.csv");
         planetData.precision(18);
      }
   #endif
}


//------------------------------------------------------------------------------
//  ~Propagate()
//------------------------------------------------------------------------------
/**
 * Destroys the Propagate Command.
 */
//------------------------------------------------------------------------------
Propagate::~Propagate()
{
   EmptyBuffer();
 
   for (UnsignedInt i=0; i<stopWhen.size(); i++)
      delete stopWhen[i];

   if (pubdata)
      delete [] pubdata;
   
   for (std::vector<PropSetup*>::iterator ps = prop.begin(); ps != prop.end(); 
        ++ps)
   {
      PropSetup *oldPs = *ps;
      *ps = NULL;
      delete oldPs;
   }
   
   for (std::vector<StringArray*>::iterator i = satName.begin(); 
        i != satName.end(); ++i)
      delete (*i);

   #ifdef DUMP_PLANET_DATA
      if (planetData.is_open())
         planetData.close();
   #endif
}


//------------------------------------------------------------------------------
//  Propagate(const Propagate &prp)
//------------------------------------------------------------------------------
/**
 * Constructs a Propagate Command based on another instance (copy constructor).
 *
 * @param <p> Original we are copying
 */
//------------------------------------------------------------------------------
Propagate::Propagate(const Propagate &prp) :
   GmatCommand                 (prp),
   propName                    (prp.propName),
   direction                   (prp.direction),
   satName                     (prp.satName),
   currentPropMode             (prp.currentPropMode),
   interruptCheckFrequency     (prp.interruptCheckFrequency),
   inProgress                  (false),
   hasFired                    (false),
   epochID                     (prp.epochID),
   stopInterval                (0.0),
   stopTrigger                 (-1),
   stopSatNames                (prp.stopSatNames),
   objectArray                 (prp.objectArray),
   elapsedTime                 (prp.elapsedTime),
   currEpoch                   (prp.currEpoch),
   state                       (NULL),
   pubdata                     (NULL),
   stopCondMet                 (false),
   stopEpoch                   (prp.stopEpoch),
   stopAccuracy                (prp.stopAccuracy),
   timeAccuracy                (prp.timeAccuracy),
   dim                         (prp.dim),
   singleStepMode              (prp.singleStepMode),
   transientForces             (NULL),
   currentMode                 (prp.currentMode),
   stopCondEpochID             (prp.stopCondEpochID),
   stopCondBaseEpochID         (prp.stopCondBaseEpochID),
   stopCondStopVarID           (prp.stopCondStopVarID)
{
   parameterCount = prp.parameterCount;
   initialized = false;
   baseEpoch.clear();
   prop.clear();
   sats.clear();
   stopWhen.clear();
   stopSats.clear();
   satBuffer.clear();
   formBuffer.clear();
   p.clear();
   fm.clear();
   stepBrackets[0] = stepBrackets[1] = 0.0;
}


//------------------------------------------------------------------------------
//  Propagate& operator=(const Propagate &prp)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Propagate Command.
 *
 * @param <p> Original we are copying
 *
 * @return reference to this copy
 */
//------------------------------------------------------------------------------
Propagate& Propagate::operator=(const Propagate &prp)
{
   if (&prp == this)
      return *this;

   // Call the base assignment operator
   GmatCommand::operator=(prp);
   
   propName                = prp.propName;
   direction               = prp.direction;
   satName                 = prp.satName;
   currentPropMode         = prp.currentPropMode;
   interruptCheckFrequency = prp.interruptCheckFrequency;
   inProgress              = false;
   hasFired                = false;   
   epochID                 = prp.epochID;   
   objectArray             = prp.objectArray;
   elapsedTime             = prp.elapsedTime;
   currEpoch               = prp.currEpoch;
   state                   = NULL;
   pubdata                 = NULL;
   stopCondMet             = false;
   stopEpoch               = prp.stopEpoch;
   stopAccuracy            = prp.stopAccuracy;
   timeAccuracy            = prp.timeAccuracy;
   dim                     = prp.dim;
   singleStepMode          = prp.singleStepMode;
   currentMode             = prp.currentMode;
   stopCondEpochID         = prp.stopCondEpochID;
   stopCondBaseEpochID     = prp.stopCondBaseEpochID;
   stopCondStopVarID       = prp.stopCondStopVarID;
   initialized             = false;
       
   baseEpoch.clear();

   for (std::vector<PropSetup*>::iterator ps = prop.begin(); ps != prop.end(); 
        ++ps)
   {
      PropSetup *oldPs = *ps;
      *ps = NULL;
      delete oldPs;
   }
   prop.clear();

   sats.clear();
   stopWhen.clear();
   stopSats.clear();
   satBuffer.clear();
   formBuffer.clear();
   p.clear();
   fm.clear();

   stepBrackets[0] = stepBrackets[1] = 0.0;
   
   return *this;
}


//------------------------------------------------------------------------------
// bool SetObject(const std::string &name, const Gmat::ObjectType type,
//         const std::string &associate, const Gmat::ObjectType associateType)
//------------------------------------------------------------------------------
/**
 * Sets objects referenced by the Propagate command
 *
 * @param <name> Name of the reference object.
 * @param <type> Type of the reference object.
 * @param <associate> Object associated with this reference object.
 * @param <associateType> Type of the associated object.
 *
 * @return true if the reference was set, false if not.
 */
//------------------------------------------------------------------------------
bool Propagate::SetObject(const std::string &name, const Gmat::ObjectType type,
                          const std::string &associate,
                          const Gmat::ObjectType associateType)
{
   Integer propNum = propName.size() - 1;
   
   switch (type) {
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
         (satName[propNum])->push_back(name);
         return true;
   
      case Gmat::PROP_SETUP:
         propName.push_back(name);
         if (name[0] == '-')
         {
            direction = -1.0;
            MessageInterface::ShowMessage("Please use the keyword \"BackProp\" "
               "to set backwards propagation; the use of a minus sign in the "
               "string \"%s\" is deprecated.\n", name.c_str());
         }
         satName.push_back(new StringArray);
         return true;
   
      default:
         break;
   }

   return GmatCommand::SetObject(name, type, associate, associateType);
}


//------------------------------------------------------------------------------
// bool SetObject(GmatBase *obj, const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Sets objects referenced by the Propagate command
 *
 * @param <name> Name of the reference object.
 * @param <type> Type of the reference object.
 *
 * @return true if the reference was set, false if not.
 */
//------------------------------------------------------------------------------
bool Propagate::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
   switch (type)
   {
   case Gmat::STOP_CONDITION:
      stopWhen.push_back((StopCondition *)obj);

      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage("Adding stopping condition named %s\n",
            obj->GetName().c_str());
      #endif

      stopCondEpochID = obj->GetParameterID("Epoch");
      stopCondBaseEpochID = obj->GetParameterID("BaseEpoch");
      stopCondStopVarID = obj->GetParameterID("StopVar");
      return true;
            
   default:
      break;
   }

   return GmatCommand::SetObject(obj, type);
}

//------------------------------------------------------------------------------
// void ClearObject(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Clears the lists of objects referenced by the Propagate command.
 *
 * @param <type> Type of the objects to clear.
 */
//------------------------------------------------------------------------------
void Propagate::ClearObject(const Gmat::ObjectType type)
{
   switch (type)
   {
   case Gmat::SPACECRAFT:
   case Gmat::FORMATION:
      satName.clear();
      break;
   case Gmat::STOP_CONDITION:
      stopWhen.clear();
      break;
            
   default:
      break;
   }
}


//------------------------------------------------------------------------------
// GmatBase* GetObject(const Gmat::ObjectType type, const std::string objName)
//------------------------------------------------------------------------------
/**
 * Accesses objects referenced by the Propagate command.
 *
 * @param <type> Type of the reference object.
 * @param <objName> Name of the reference object.
 *
 * @return true if the reference was set, false if not.
 */
//------------------------------------------------------------------------------
GmatBase* Propagate::GetObject(const Gmat::ObjectType type,
                               const std::string objName)
{
   if (type == Gmat::STOP_CONDITION)
      if (stopWhen.empty())
         return NULL;
      else
         return stopWhen[0];
            
   return GmatCommand::GetObject(type, objName);
}

//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& Propagate::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   std::string gen = prefix + "Propagate";
   
   // Construct the generating string
   UnsignedInt index = 0;
   
   if (direction < 0.0)
      gen += (" BackProp");
   
   if (currentPropMode != "")
      gen += (" " + currentPropMode);
   for (StringArray::iterator prop = propName.begin(); prop != propName.end();
        ++prop) {
      gen += " " + (*prop) + "(";
      // Spaceobjects that are propagated by this PropSetup
      StringArray *sats = satName[index];
      for (StringArray::iterator sc = sats->begin(); sc != sats->end(); ++sc) {
         // Add a comma if needed
         if (sc != sats->begin())
            gen += ", ";
         gen += (*sc);
      }

      gen += ")";
      ++index;
   }

   // Now the stopping conditions.  Note that stopping conditions are shown at
   // the end of the Propagate line, rather than inside of the PropSetup 
   // delimiters.
   if (stopWhen.size() > 0) {
      gen += " {";
   
      for (std::vector<StopCondition*>::iterator stp = stopWhen.begin();
           stp != stopWhen.end(); ++stp) {
         std::stringstream stopCondDesc;
         if (stp != stopWhen.begin())
            gen += ", ";
   
         std::string stopName = (*stp)->GetStringParameter(stopCondStopVarID);
         stopCondDesc << stopName;
   
         if ((stopName.find(".Periapsis") == std::string::npos) &&
             (stopName.find(".Apoapsis") == std::string::npos))
            stopCondDesc << " = " << (*stp)->GetStringParameter("Goal");
   
         gen += stopCondDesc.str();
      }
      
      // Add the stop tolerance is it is not set to the default value
      if (stopAccuracy != DEFAULT_STOP_TOLERANCE)
      {
         gen += ", StopTolerance = ";
         std::stringstream stopTolDesc;
         stopTolDesc.precision(13);
         stopTolDesc << stopAccuracy;
         gen += stopTolDesc.str();
      }
      gen += "}";
   }

   generatingString = gen + ";";
   // Then call the base class method
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Propagate.
 *
 * @return clone of the Propagate.
 */
//------------------------------------------------------------------------------
GmatBase* Propagate::Clone() const
{
   #ifdef DEBUG_CLONE
   MessageInterface::ShowMessage("Propagate::Clone() entered\n");
   #endif
   return (new Propagate(*this));
}


//------------------------------------------------------------------------------
//  std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Accessor used to find the names of referenced objects.
 *
 * @param <type> reference object type.
 *
 * @return The name of the reference object.
 */
//------------------------------------------------------------------------------
std::string Propagate::GetRefObjectName(const Gmat::ObjectType type) const
{
   /// @todo Figure out how to send back the arrays of names
   switch (type) {
      // Propagator setups
      case Gmat::PROP_SETUP:
         return propName[0];
      
      // Objects that get propagated
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
         if (satName.size() > 0)
            return (*satName[0])[0];
      
      default:
         break;
   }
   
   return GmatCommand::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accessor used to set the names of referenced objects.
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Propagate::SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name)
{
   switch (type) {
      // Propagator setups
      case Gmat::PROP_SETUP:
         propName.push_back(name);
         satName.push_back(new StringArray);
         return true;
      
      // Objects that get propagated
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION: 
      {
         Integer propNum = propName.size()-1;
         satName[propNum]->push_back(name);
         return true;
      }
      
      default:
         break;
   }
   
   return GmatCommand::SetRefObjectName(type, name);
}

// Reference object accessor methods
//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name,
//                        const Integer index
//------------------------------------------------------------------------------
/**
 * Accessor for reference object pointers.
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* Propagate::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name, const Integer index)
{
   switch (type)
   {
   case Gmat::STOP_CONDITION:
      if (index < (Integer)stopWhen.size())
      {
         return stopWhen[index];
      }
      else
      {
         throw CommandException("Propagate::GetRefObject() invalid index\n");
      }
   default:
      break;
   }

   return GmatCommand::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type, ...
//------------------------------------------------------------------------------
/**
 * Sets reference object pointer.
 *
 * @param <obj> Pointer to the reference object.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return true if object successfully set, false otherwise
 */
//------------------------------------------------------------------------------
bool Propagate::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name, const Integer index)
{
   #if DEBUG_PROPAGATE_OBJ
      MessageInterface::ShowMessage
         ("Propagate::SetRefObject() type=%s name=%s, index=%d\n",
          obj->GetTypeName().c_str(), name.c_str(), index);
   #endif
   
   switch (type)
   {
      case Gmat::STOP_CONDITION:
         {
            std::string satName = obj->GetName();
            Integer strt = satName.find("StopOn") + 6;
            if (strt == (Integer)std::string::npos)
               strt = 0;
            Integer ndx = satName.find(".",0);
            if (ndx != (Integer)std::string::npos)
               satName = satName.substr(strt, ndx-strt);

            Integer size = stopWhen.size();

            if (stopWhen.empty() && index == 0)
            {
               stopWhen.push_back((StopCondition *)obj);
               stopSatNames.push_back(satName);
            }
            else if (index == size)
            {
               stopWhen.push_back((StopCondition *)obj);
               stopSatNames.push_back(satName);
            }
            else if (index < size)
            {
               stopWhen[index] = (StopCondition *)obj;
               stopSatNames[index] = satName;
            }
            else
            {
               MessageInterface::ShowMessage
                  ("Propagate::SetRefObject() index=%d is not next available "
                   "index=%d. Setting %s:%s failed\n", index, size,
                   obj->GetTypeName().c_str(), obj->GetName().c_str());
               return false;
            }

            #if DEBUG_PROPAGATE_OBJ
               for (UnsignedInt  j=0; j<stopSatNames.size(); j++)
                  MessageInterface::ShowMessage(
                     "Propagate::SetRefObject() stopSatNames=%s\n",
                     stopSatNames[j].c_str());
            #endif
            
            #ifdef DEBUG_STOPPING_CONDITIONS
               MessageInterface::ShowMessage(
                  "Adding stopping condition named %s\n",
                  obj->GetName().c_str());
            #endif

            stopCondEpochID = obj->GetParameterID("Epoch");
            stopCondBaseEpochID = obj->GetParameterID("BaseEpoch");
            stopCondStopVarID = obj->GetParameterID("StopVar");
            return true;
         }
         
      default:
         break;
   }

   return GmatCommand::SetRefObject(obj, type, name, index);
}

//------------------------------------------------------------------------------
// virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param type The type of objects requested.
 *
 * @return Reference to the array.
 */
//------------------------------------------------------------------------------
ObjectArray& Propagate::GetRefObjectArray(const Gmat::ObjectType type)
{
   objectArray.clear();
   
   switch (type)
   {
      case Gmat::STOP_CONDITION:
         for (UnsignedInt i=0; i<stopWhen.size(); i++)
            objectArray.push_back(stopWhen[i]);
         return objectArray;
      
      default:
         break;
   }

   return GmatCommand::GetRefObjectArray(type);
}


// Parameter accessor methods

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the description for the parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return String description for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Propagate::GetParameterText(const Integer id) const
{
   if ((id < PropagateCommandParamCount) && (id >= GmatCommandParamCount))
      return PARAMETER_TEXT[id - GmatCommandParamCount];

   return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieve the ID for the parameter given its description.
 *
 * @param <str> Description for the parameter.
 *
 * @return the parameter ID.
 */
//------------------------------------------------------------------------------
Integer Propagate::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < PropagateCommandParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }

   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Propagate::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < PropagateCommandParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
      
   return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the string associated with a parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return Text description for the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Propagate::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Integer Propagate::GetIntegerParameter(const Integer id) const
{
   if (id == INTERRUPT_FREQUENCY)
      return interruptCheckFrequency;

   return GmatCommand::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Set the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call.
 */
//------------------------------------------------------------------------------
Integer Propagate::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == INTERRUPT_FREQUENCY) {
      if (value >= 0)
         interruptCheckFrequency = value;
      return interruptCheckFrequency;
   }

   return GmatCommand::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
std::string Propagate::GetStringParameter(const Integer id) const
{
   if (id == PROP_COUPLED)
      return currentPropMode;

   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored.
 */
//------------------------------------------------------------------------------
bool Propagate::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == PROP_COUPLED) 
   {
      const StringArray pmodes = GetStringArrayParameter(AVAILABLE_PROP_MODES);
      if (find(pmodes.begin(), pmodes.end(), value) != pmodes.end()) 
      {
         // Back prop is a special case
         if (value == "BackProp")
         {
            direction = -1.0;
         }
         else
         {
            currentPropMode = value;
            for (Integer i = 0; i < PropModeCount; ++i)
               if (value == pmodes[i]) 
               {
                  currentMode = (PropModes)i;
                  return true;
               }
         }
      }
   }
 
   if (id == SAT_NAME) 
   {
      Integer propNum = propName.size()-1;
      satName[propNum]->push_back(value);
      return true;
   }

   if (id == PROP_NAME) 
   {
      std::string propNameString = value;
      if (propNameString[0] == '-')
      {
         direction = -1.0;
         MessageInterface::ShowMessage("Please use the keyword \"BackProp\" to "
            "set backwards propagation; the use of a minus sign in the string "
            "\"%s\" is deprecated.\n", propNameString.c_str());
         propNameString = propNameString.substr(1);
      }
      propName.push_back(propNameString);
      satName.push_back(new StringArray);
      return true;
   }
 
   return GmatCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool Propagate::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{   
   if (id == SAT_NAME) {
      if (index < (Integer)propName.size())
         satName[index]->push_back(value);
      else
         throw CommandException("Propagate::SetStringParameter Attempting to "
                         "assign a spacecraft without an associated PropSetup");
      return true;
   }

   return GmatCommand::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& Propagate::GetStringArrayParameter(const Integer id) const
{
   static StringArray modeList;
   
   if (id == AVAILABLE_PROP_MODES) {
      modeList.clear();
      for (Integer i = 0; i < PropModeCount; ++i)
         // BackProp isn't really a prop sync mode
         if (PropModeList[i] != "BackProp")
            modeList.push_back(PropModeList[i]);
      return modeList;
   }
 
   if (id == SAT_NAME)
      return *satName[0];
 
   if (id == PROP_NAME) {
      return propName;
   }
 
   return GmatCommand::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id, 
//                                            const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& Propagate::GetStringArrayParameter(const Integer id,
                                               const Integer index) const
{
   if (id == SAT_NAME)
      return *satName[index];

   return GmatCommand::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// Integer GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for an Boolean parameter.
 * 
 * Propagate currently contains the following Boolean parameter:
 * 
 *    PROP_FORWARDS     Evaluates true if the prop direction is forward in time,
 *                      false if propagating backwards.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
bool Propagate::GetBooleanParameter(const Integer id) const
{
   if (id == PROP_FORWARD)
      return (direction > 0.0 ? true : false);
      
   return GmatCommand::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Set the value for an Boolean parameter.
 * 
 * Propagate currently contains the following Boolean parameter:
 * 
 *    PROP_FORWARDS     Set true if the prop direction is forward in time,
 *                      false if propagating backwards.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The parameter setting
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
bool Propagate::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == PROP_FORWARD)
   {
      if (value == true)
         direction = 1.0;
      else
         direction = -1.0;
      return true;
   }

   return GmatCommand::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// Integer GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for an Boolean parameter.
 * 
 * Propagate currently contains the following Boolean label:
 * 
 *    "PropForward"     Evaluates true if the prop direction is forward in time,
 *                      false if propagating backwards.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
bool Propagate::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Set the value for an Boolean parameter.
 * 
 * Propagate currently contains the following Boolean parameter:
 * 
 *    "PropForward"     Set true if the prop direction is forward in time,
 *                      false if propagating backwards.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The parameter setting
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
bool Propagate::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


Real Propagate::GetRealParameter(const Integer id) const
{
   if (id == STOP_ACCURACY)
      return stopAccuracy;
   return GmatCommand::GetRealParameter(id);
}

Real Propagate::SetRealParameter(const Integer id, const Real value)
{
   if (id == STOP_ACCURACY)
   {
      if (value > 0.0)
      {
         stopAccuracy = value;
         firstStepTolerance = stopAccuracy * 10.0;
      }
      else
      {
         std::stringstream val;
         val.precision(13);
         val << value;
         CommandException ce;
         ce.SetDetails(errorMessageFormatUnnamed.c_str(),
            val.str().c_str(), "StopTolerance", "a Real number > 0.0");
         throw ce;
      }
      return stopAccuracy;
   }
   return GmatCommand::SetRealParameter(id, value);
}

Real Propagate::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

Real Propagate::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <action> The string descriptor for the requested action.
 * @param <actionData> Optional data used for the action.
 *
 * @return true if the action was performed, false if not.
 */
//------------------------------------------------------------------------------
bool Propagate::TakeAction(const std::string &action,
                           const std::string &actionData)
{
   if (action == "Clear")
   {
      if (actionData == "Propagator")
      {
         for (Integer i = 0; i < (Integer)satName.size(); ++i)
         {
            delete satName[i];
         }
         satName.clear();

         propName.clear();

         for (std::vector<PropSetup*>::iterator ps = prop.begin(); 
              ps != prop.end(); ++ps)
         {
            PropSetup *oldPs = *ps;
            *ps = NULL;
            delete oldPs;
         }
         prop.clear();
         p.clear();
         fm.clear();

         sats.clear();
      }
      else if (actionData == "StopCondition")
      {
         stopWhen.clear();
         stopSats.clear();
         stopSatNames.clear();
         return true;
      }
   }
   else if (action == "SetStopSpacecraft")
   {
      stopSatNames.push_back(actionData);
      return true;
   }
   else if (action == "ResetLoopData")
   {
      for (std::vector<Propagator*>::iterator i = p.begin(); i != p.end(); ++i)
      {
         (*i)->ResetInitialData(); 
      }
      return true;
   }
   
   return GmatCommand::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Propagate::RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Propagate::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   // Propagate needs to know about spacecraft or formation only
   if (type != Gmat::SPACECRAFT && type != Gmat::FORMATION &&
       type != Gmat::PROP_SETUP && type != Gmat::PARAMETER)
      return true;
   
   StringArray::iterator pos;
   
   if (type == Gmat::PROP_SETUP)
   {
      // rename PropSetup
      for (pos = propName.begin(); pos != propName.end(); ++pos)
         if (*pos == oldName)
            *pos = newName;
   }
   else
   {
      // rename space object name used in prop setup
      for (UnsignedInt prop = 0; prop < propName.size(); ++prop)
         for (pos = satName[prop]->begin(); pos != satName[prop]->end(); ++pos)
            if (*pos == oldName)
               *pos = newName;
      
      // rename space object name used in stopping condition
      for (UnsignedInt i = 0; i < stopSatNames.size(); ++i)
         if (stopSatNames[i] == oldName)
            stopSatNames[i] = newName;
      
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("Propagate::RenameRefObject() Rename StopCondtion Object\n");
      #endif
      
      // rename stop condition parameter
      for (UnsignedInt i=0; i<stopWhen.size(); i++)
         stopWhen[i]->RenameRefObject(type, oldName, newName);
   }
   
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Propagate.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Propagate::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::PROP_SETUP);
   refObjectTypes.push_back(Gmat::SPACECRAFT);
   refObjectTypes.push_back(Gmat::PARAMETER);   
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the Propagate.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& Propagate::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::PROP_SETUP)
   {
      // Remove backward prop notation '-'
      std::string newPropName;
      for (UnsignedInt i=0; i<propName.size(); i++)
      {
         newPropName = propName[i];
         if (newPropName[0] == '-')
            newPropName = propName[i].substr(1);
      
         refObjectNames.push_back(newPropName);
      }      
   }
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACECRAFT)
   {
      refObjectNames.insert(refObjectNames.end(), stopSatNames.begin(),
                            stopSatNames.end());
   }
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::PARAMETER)
   {
      refObjectNames.insert(refObjectNames.end(), stopNames.begin(),
                            stopNames.end());
   }
   
   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Propagate command has the following syntax:
 *
 *     Propagate prop(Sat1, ... , {Sat1.ElapsedDays = 700}) ...;
 *
 * where prop is a PropSetup, "Sat1, ... ," is the list of SpaceObjects that are
 * propagated, and the items in curly braces are the (optional) stopping
 * conditions.  The Propagate command supports simultaneous propagation of
 * multiple spacecraft, either in a single PropSetup or in a list of PropSetups
 * on the same Propagate command line.
 *
 * This method breaks the script line into the corresponding pieces, and stores
 * the names of the PropSetups, SpaceObjects, and StoppingConditions so they can
 * be set to point to the correct objects during initialization.
 *
 * @return true on successful parsing of the command.
 */
//------------------------------------------------------------------------------
bool Propagate::InterpretAction()
{
   #ifdef DEBUG_PROPAGATE_ASSEMBLE
   MessageInterface::ShowMessage
      ("Propagate::InterpretAction() genString = \"%s\"\n",
       generatingString.c_str());
   #endif
   
   Integer loc = generatingString.find("Propagate", 0) + 9;
   const char *str = generatingString.c_str();
   
   if (generatingString.find("..") != generatingString.npos)
      throw CommandException("Propagate::InterpretAction: Can not parse command\n "
                                + generatingString);
   
   while (str[loc] == ' ')
      ++loc;

   // Check to see if there are optional parameters (e.g. "Synchronized")
   CheckForOptions(loc, generatingString);
   // Now fill in the list of propagators
   AssemblePropagators(loc, generatingString);
   
   return true;
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray()
//------------------------------------------------------------------------------
const StringArray& Propagate::GetWrapperObjectNameArray()
{
   wrapperObjectNames.clear();
   wrapperObjectNames.insert(wrapperObjectNames.end(), stopNames.begin(),
                             stopNames.end());
   wrapperObjectNames.insert(wrapperObjectNames.end(), goalNames.begin(),
                             goalNames.end());
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Propagate::SetElementWrapper(ElementWrapper *toWrapper,
                                  const std::string &withName)
{
   #ifdef DEBUG_WRAPPERS   
   MessageInterface::ShowMessage
      ("Propagate::SetElementWrapper() entered with toWrapper=<%p>, withName='%s'\n",
       toWrapper, withName.c_str());
   #endif
   
   if (toWrapper == NULL)
      return false;
   
   // this would be caught by next part, but this message is more meaningful
   if (toWrapper->GetWrapperType() == Gmat::ARRAY)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   
   CheckDataType(toWrapper, Gmat::REAL_TYPE, "Propagate", true);
   
   bool retval = false;
   ElementWrapper *ew;
   
   //-------------------------------------------------------
   // check stopping condition names
   //-------------------------------------------------------
   #ifdef DEBUG_WRAPPERS   
   MessageInterface::ShowMessage
      ("   Checking %d Propagate Stop Conditions\n", stopNames.size());
   for (UnsignedInt i=0; i<stopNames.size(); i++)
      MessageInterface::ShowMessage("      %s\n", stopNames[i].c_str());
   #endif
   
   Integer sz = stopNames.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (stopNames.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPERS   
         MessageInterface::ShowMessage
            ("   Found wrapper name \"%s\" in stopNames\n", withName.c_str());
         #endif
         
         for (UnsignedInt j=0; j<stopWhen.size(); j++)
         {
            #ifdef DEBUG_WRAPPERS   
            MessageInterface::ShowMessage
               ("   stopWhen[j]->GetName()='%s'\n", stopWhen[j]->GetName().c_str());
            #endif
            if (stopWhen[j]->GetName() == ("StopOn" + withName))
               stopWhen[j]->SetStopParameter((Parameter*)toWrapper->GetRefObject());
         }
         
         if (stopWrappers.at(i) != NULL)
         {
            ew = stopWrappers.at(i);
            stopWrappers.at(i) = toWrapper;
            
            // Delete old wrapper if wrapper name not found in the goalNames
            if (find(goalNames.begin(), goalNames.end(), withName) == goalNames.end())
               delete ew;
         }
         else
            stopWrappers.at(i) = toWrapper;
         
         retval = true;
      }
   }
   
   //-------------------------------------------------------
   // check goal names
   //-------------------------------------------------------
   #ifdef DEBUG_WRAPPERS   
   MessageInterface::ShowMessage
      ("   Checking %d Propagate Stop Goals\n", goalNames.size());
   for (UnsignedInt i=0; i<goalNames.size(); i++)
      MessageInterface::ShowMessage("      %s\n", goalNames[i].c_str());
   #endif
   
   sz = goalNames.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (goalNames.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPERS   
         MessageInterface::ShowMessage
            ("   Found wrapper name \"%s\" in goalNames\n", withName.c_str());
         #endif
         if (goalWrappers.at(i) != NULL)
         {
            ew = goalWrappers.at(i);
            goalWrappers.at(i) = toWrapper;
            
            // Delete old wrapper if wrapper name not found in the stopNames
            if (find(stopNames.begin(), stopNames.end(), withName) == stopNames.end())
               delete ew;
         }
         else
            goalWrappers.at(i) = toWrapper;
         retval = true;
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// void CheckForOptions(Integer &loc, std::string &generatingString)
//------------------------------------------------------------------------------
/**
 * Looks for propagator options that exist prior to any PropSetup names.
 *
 * @param <loc>               The current location in the generating string.
 * @param <generatingString>  The generating string.
 */
//------------------------------------------------------------------------------
void Propagate::CheckForOptions(Integer &loc, std::string &generatingString)
{
   std::string modeStr;
   currentMode = INDEPENDENT;
   
   for (Integer modeId = INDEPENDENT+1; modeId != PropModeCount; ++modeId) 
   {
      modeStr = PropModeList[modeId];
      modeStr += " ";
      
      #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("Propagate::CheckForOptions() looking"
                                " for \"%s\" starting at loc=%d in \n\"%s\"",
                                modeStr.c_str(), loc, generatingString.c_str());
      #endif
      
      Integer end = generatingString.find(modeStr, loc);
      if (end != (Integer)std::string::npos) 
      {
         if (modeStr == "BackProp ")
         {
            #ifdef DEBUG_PROPAGATE_ASSEMBLE
               MessageInterface::ShowMessage("\nDirection is now %d\n", currentMode);
            #endif
   
            direction = -1.0;
         }
         else
         {
            currentMode = (PropModes)modeId;
            currentPropMode = PropModeList[modeId];
            #ifdef DEBUG_PROPAGATE_ASSEMBLE
               MessageInterface::ShowMessage("\nLocated at %d\n", end);
               MessageInterface::ShowMessage("Mode is now %d\n", currentMode);
            #endif
         }
   
         if (end >= loc)
            loc = end + modeStr.length();
      }
      else
      {
         #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("\n");
         #endif
      }
   }
}


//------------------------------------------------------------------------------
// void AssemblePropagators(Integer &loc, std::string& generatingString)
//------------------------------------------------------------------------------
/**
 * Parses the PropSetup portion of the Propagate command.
 *
 * @param <loc>               The current location in the generating string.
 * @param <generatingString>  The generating string.
 */
//------------------------------------------------------------------------------
void Propagate::AssemblePropagators(Integer &loc, 
   std::string& generatingString)
{
   // First parse the pieces from the string, starting at loc
   StringArray setupStrings, stopStrings;
   
   FindSetupsAndStops(loc, generatingString, setupStrings, stopStrings);

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      // Output the chunks for debugging
      MessageInterface::ShowMessage("PropSetups:\n");
      for (StringArray::iterator i = setupStrings.begin(); 
           i != setupStrings.end(); ++i)
         MessageInterface::ShowMessage("   '%s'\n", i->c_str());
      MessageInterface::ShowMessage("StopConditions:\n");
      for (StringArray::iterator i = stopStrings.begin(); 
           i != stopStrings.end(); ++i)
         MessageInterface::ShowMessage("   '%s'\n", i->c_str());
   #endif
   
   // Now build the prop setups
   for (StringArray::iterator i = setupStrings.begin(); 
        i != setupStrings.end(); ++i)
      ConfigurePropSetup(*i);
   
   // and the stopping conditions
   for (StringArray::iterator i = stopStrings.begin(); 
        i != stopStrings.end(); ++i)
      ConfigureStoppingCondition(*i);

   // Finally, set the prop mode
   if (stopWhen.empty())
      singleStepMode = true;  // If not, run in single step mode
}


//------------------------------------------------------------------------------
// void Propagate::FindSetupsAndStops(Integer &loc, 
//   std::string& generatingString, StringArray &setupStrings, 
//   StringArray &stopStrings)
//------------------------------------------------------------------------------
/**
 * Breaks out the PropSetup object strings and the stopping condition strings.
 *
 * @param <loc>               The current location in the generating string.
 * @param <generatingString>  The generating string.
 * @param <setupStrings>      The container for the PropSetup strings.
 * @param <stopStrings>       The container for teh stopping condition strings.
 */
//------------------------------------------------------------------------------
void Propagate::FindSetupsAndStops(Integer &loc, 
   std::string& generatingString, StringArray &setupStrings, 
   StringArray &stopStrings)
{
   // First parse the pieces from the string, starting at loc
   std::string tempString, setupWithStop, oneStop;
   const char *str = generatingString.c_str();
   Integer currentLoc = loc, parmstart, end, commaLoc;

   bool scanning = true;
   
   // First find the PropSetups
   parmstart = generatingString.find("(", currentLoc);   
   while (scanning) 
   {
      //end = generatingString.find(")", parmstart)+1;
      end = generatingString.find(")", parmstart);

      if (end == (Integer)std::string::npos)
         throw CommandException("Propagate::AssemblePropagators: Propagate"
                                " string does not identify propagator");
      ++end;
      
      if (generatingString[currentLoc] == '-') 
      {
         direction = -1.0;
         MessageInterface::ShowMessage("Please use the keyword \"BackProp\" to "
            "set backwards propagation; the use of a minus sign in the string "
            "\"%s\" is deprecated.\n", generatingString.c_str());
         ++currentLoc;
      }
        
      tempString = generatingString.substr(currentLoc, end-currentLoc);
      // Remove stop condition here
      if (tempString.find("{", 0) != std::string::npos)
      {
         setupWithStop = tempString;
         
         Integer braceStart = setupWithStop.find("{", 0),
                 braceEnd   = setupWithStop.find("}", 0);
                 
         if (braceEnd == (Integer)std::string::npos) 
            throw CommandException("Propagate::AssemblePropagators: PropSetup"
                                  " string " + tempString +
                                  " starts a stopping condition, but does not"
                                  " have a closing brace.");
         // Now remove the bracketed chunk from the string
         tempString = setupWithStop.substr(0, braceStart);
         // Remove the comma
         Integer commaLoc = braceStart - 1;
         while ((tempString[commaLoc] == ',') || (tempString[commaLoc] == ' '))
         {
            --commaLoc;
         }
         tempString = tempString.substr(0, commaLoc+1);
         
         // Add on the trailing chunk
         tempString += setupWithStop.substr(braceEnd+1);
      }
            
      setupStrings.push_back(tempString);
      currentLoc = end+1;

      // Skip trailing comma or white space
      while ((str[currentLoc] == ',') || (str[currentLoc] == ' '))
         ++currentLoc;
      parmstart = generatingString.find("(", currentLoc);
      if (parmstart == (Integer)std::string::npos)
         scanning = false;
   }
   
   // Now find the stopping conditions
   scanning = true;
   currentLoc = loc;
   
   parmstart = generatingString.find("{", currentLoc);
   if ((std::string::size_type)parmstart == std::string::npos)
      scanning = false;
   
   while (scanning) 
   {
      end = generatingString.find("}", parmstart)+1;
      
      if (end == (Integer)std::string::npos+1)
      { 
         throw CommandException("Propagate::AssemblePropagators: PropSetup"
                                  " string " + generatingString +
                                  " starts a stopping condition, but does not"
                                  " have a closing brace.");
      }
            
      tempString = generatingString.substr(parmstart+1, end-parmstart-2);
      
      // Split out stops, one at a time
      currentLoc = 0;
      do 
      {
         commaLoc = tempString.find(",", currentLoc);
         oneStop = tempString.substr(currentLoc, commaLoc - currentLoc);
         // Remove leading white space
         while (oneStop[0] == ' ')
            oneStop = oneStop.substr(1);
         // Remove trailing white space
         currentLoc = oneStop.length() - 1;
         while (oneStop[currentLoc] == ' ')
            --currentLoc;
         oneStop = oneStop.substr(0, currentLoc+1);
         stopStrings.push_back(oneStop);

         currentLoc = commaLoc + 1;
      } while (commaLoc != (Integer)std::string::npos);

      currentLoc = end+1;
      
      // Skip trailing comma or white space
      while ((str[currentLoc] == ',') || (str[currentLoc] == ' '))
         ++currentLoc;
      parmstart = generatingString.find("{", currentLoc);
      if (parmstart == (Integer)std::string::npos)
         scanning = false;
   }
}


//------------------------------------------------------------------------------
// void ConfigurePropSetup(std::string &setupDesc)
//------------------------------------------------------------------------------
/**
 * Builds the data needed for the a PropSetup.  Stopping conditions are handled
 * separately.
 *
 * @param <setupDesc>  The string describing the PropSetup.
 */
//------------------------------------------------------------------------------
void Propagate::ConfigurePropSetup(std::string &setupDesc)
{
   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("Building PropSetup '%s'\n", 
         setupDesc.c_str());
   #endif

   // First separate the PropSetup from the SpaceObjects
   std::string prop, sats, sat;
   std::string::size_type loc = setupDesc.find("(");
   if (loc == std::string::npos)
      throw CommandException("The propsetup string '" + setupDesc +
         "' does not identify any spacecraft for propagation on "
         + "the command line\n" + generatingString);
   prop = setupDesc.substr(0, loc);
   sats = setupDesc.substr(loc);
   
   CleanString(prop);

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("   PropSetup is '%s'\n", prop.c_str());
   #endif
   SetObject(prop, Gmat::PROP_SETUP);
   
   // Next the SpaceObjects
   StringArray extras;
   extras.push_back("(");
   extras.push_back(")");
   extras.push_back(",");
 
   loc = 0;
   while (loc != std::string::npos)
   {  
      loc = sats.find(',');
      sat = sats.substr(0, loc);
      sats = sats.substr(loc+1);
      CleanString(sat, &extras);
      
      #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("   Found satellite '%s'\n", sat.c_str());
      #endif
      SetObject(sat, Gmat::SPACECRAFT);
   }
}


//------------------------------------------------------------------------------
// void ConfigureStoppingCondition(std::string &stopDesc)
//------------------------------------------------------------------------------
/**
 * Builds the data needed for a stopping condition.  PropSetups are handled
 * separately.
 *
 * @param <stopDesc>  The string describing the stopping condition.
 */
//------------------------------------------------------------------------------
void Propagate::ConfigureStoppingCondition(std::string &stopDesc)
{
   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("Building Stop '%s'\n", 
         stopDesc.c_str());
   #endif

   std::string lhs, rhs = "";
   std::string::size_type loc;
   StringArray extras;
   extras.push_back("{");
   extras.push_back("}");
   extras.push_back("=");
   
   loc = stopDesc.find("=");
   if (loc == std::string::npos)
   {
      lhs = stopDesc;
      CleanString(lhs, &extras);
   }
   else
   {
      lhs = stopDesc.substr(0,loc);
      CleanString(lhs, &extras);
      rhs = stopDesc.substr(loc+1);
      CleanString(rhs, &extras);
      
      if (lhs == "StopTolerance")
      {
         Real rval;
         if (GmatStringUtil::ToReal(rhs, rval))
            SetRealParameter(STOP_ACCURACY, rval);
         else
         {
            CommandException ce;
            ce.SetDetails(errorMessageFormatUnnamed.c_str(),
               rhs.c_str(), "StopTolerance", "a Real number > 0.0");
            throw ce;
         }
         return;
      }
   }

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("   Stop = '%s' with value '%s'\n", 
         lhs.c_str(), rhs.c_str());
   #endif
      
   // Now to work!
   std::string paramType, paramObj, paramSystem;
   GmatStringUtil::ParseParameter(lhs, paramType, paramObj, paramSystem);
   
   // Create the stop parameter
   std::string paramName;
   if (paramSystem == "")
      paramName = paramObj + "." + paramType;
   else
      paramName = paramObj + "." + paramSystem + "." + paramType;
   
   #ifdef DEBUG_PROPAGATE_ASSEMBLE
   MessageInterface::ShowMessage("   Creating local StopCondition\n");
   #endif
   StopCondition *stopCond = new StopCondition("StopOn" + paramName);
   if (find(stopNames.begin(), stopNames.end(), paramName) == stopNames.end())
   {
      #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("   Adding '%s' to stopNames\n", paramName.c_str());
      #endif
      stopNames.push_back(paramName);
      stopWrappers.push_back(NULL);
   }
   
   // Handle some static member initialization if this is the first opportunity
   if (stopCondEpochID == -1)
   {
      stopCondEpochID = stopCond->GetParameterID("Epoch");
      stopCondBaseEpochID = stopCond->GetParameterID("BaseEpoch");
      stopCondStopVarID = stopCond->GetParameterID("StopVar");
   }
   
   // Setup for backwards propagation
   stopCond->SetPropDirection(direction);  // Use direction of props
   stopCond->SetStringParameter(stopCondStopVarID, paramName);
   SetObject(stopCond, Gmat::STOP_CONDITION);
   TakeAction("SetStopSpacecraft", paramObj);
   
   
   if (paramType != "Apoapsis" && paramType != "Periapsis")
   {
      #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("Propagate::AssemblePropagators()"
            " component = <%s>\n", rhs.c_str());
      #endif
         
      // create goal parameter
      std::string component = rhs;
      
      if (find(goalNames.begin(), goalNames.end(), component) == goalNames.end())
      {
         #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("   Adding '%s' to goalNames\n", component.c_str());
         #endif
         goalNames.push_back(component);
         goalWrappers.push_back(NULL);         
      }
      
      stopCond->SetStringParameter("Goal", component);
   }
   else
   {
      if (rhs.length() != 0)
      {
         throw CommandException("Stopping condition " + paramType + 
            " does not take a value, but it is set using the string '" + 
            stopDesc + "' in the line\n'" + generatingString + "'");
      }
   }
}


//------------------------------------------------------------------------------
// void CleanString(std::string &theString, const StringArray *extras)
//------------------------------------------------------------------------------
/**
 * Strips off leading and trailing whitespace, and additional characters if 
 * specified.
 *
 * @param <theString>  The string that -- might -- need cleaned.
 * @param <extras>     All additional characters (other than a space) that 
 *                     should be stripped off.
 */
//------------------------------------------------------------------------------
void Propagate::CleanString(std::string &theString, const StringArray *extras)
{
   UnsignedInt loc, len = theString.length();
   bool keepGoing = false;
   
   // Clean up the start of the string
   for (loc = 0; loc < len; ++loc)
   {
      if (theString[loc] != ' ')
      {
         if (extras != NULL)
            for (StringArray::const_iterator i = extras->begin(); i != extras->end(); ++i)
               if (theString[loc] == (*i)[0])
                  keepGoing = true;
         if (!keepGoing)
            break;
         else
            keepGoing = false;
      }
   }
   theString = theString.substr(loc);
   
   // Clean up the end of the string
   keepGoing = false;
   for (loc = theString.length() - 1; loc >= 0; --loc)
   {
      if (theString[loc] != ' ')
      {
         if (extras != NULL)
            for (StringArray::const_iterator i = extras->begin(); i != extras->end(); ++i)
               if (theString[loc] == (*i)[0])
                  keepGoing = true;
         if (!keepGoing)
            break;
         else
            keepGoing = false;
      }
   }
   theString = theString.substr(0, loc+1);
}


//------------------------------------------------------------------------------
// void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
 * Sets the array of transient forces, so it can be passed to the PropSetups.
 *
 * @param <tf> The array of transient forces.
 */
//------------------------------------------------------------------------------
void Propagate::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   transientForces = tf;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Propagate command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Propagate::Initialize()
{
   #if DEBUG_PROPAGATE_INIT
      MessageInterface::ShowMessage("Propagate::Initialize() entered.\n%s\n",
                                    generatingString.c_str());
      MessageInterface::ShowMessage("  Size of propName is %d\n",
                                    propName.size());
      for (UnsignedInt ind = 0; ind < propName.size(); ++ind)
         MessageInterface::ShowMessage("     %d:  %s\n",
                                    propName.size(), propName[ind].c_str());
         
      MessageInterface::ShowMessage("  Direction is %s\n",
                                    (direction == 1.0?"Forwards":"Backwards"));
   #endif

   GmatCommand::Initialize();
   
   inProgress = false;
   UnsignedInt index = 0;
//   prop.clear();
   sats.clear();
   SpaceObject *so;
   std::string pName;
   GmatBase *mapObj = NULL;

   // Ensure that we are using fresh objects when buffering stops
   EmptyBuffer();
   
   // Remove old PropSetups
   for (std::vector<PropSetup*>::iterator ps = prop.begin(); ps != prop.end(); 
        ++ps)
   {
      PropSetup *oldPs = *ps;
      *ps = NULL;
      delete oldPs;
   }
   prop.clear();
   p.clear();
   fm.clear();
      
   for (StringArray::iterator i = propName.begin(); i != propName.end(); ++i)
   {
      if (satName.size() <= index)
         throw CommandException("Size mismatch for SpaceObject names\n");
         
      if ((*i)[0] == '-') 
         pName = i->substr(1);
      else 
        pName = *i;

      if ((mapObj = FindObject(pName)) == NULL)
         throw CommandException(
            "Propagate command cannot find Propagator Setup \"" + (pName) +
            "\"\n");
   
      if (satName[index]->empty())
         throw CommandException(
            "Propagate command does not have a SpaceObject for " + (pName) +
            " in \n\"" + generatingString + "\"\n");
   
      if (stopWhen.empty())
         singleStepMode = true;
      else
         singleStepMode = false;

      prop.push_back((PropSetup *)(mapObj->Clone()));
      if (!prop[index])
         return false;
      
      Propagator *p = prop[index]->GetPropagator();
      if (!p)
         throw CommandException("Propagator not set in PropSetup\n");
   
      // Toss the spacecraft into the force model
      ForceModel *fm = prop[index]->GetForceModel();
      if (!fm)
         throw CommandException("ForceModel not set in PropSetup\n");
      fm->ClearSpacecraft();
      StringArray::iterator scName;
      StringArray owners, elements;

      /// @todo Check to see if All and All.Epoch belong in place for all modes.
      owners.push_back("All");
      elements.push_back("All.epoch");
      
      bool finiteBurnActive = false;
      
      for (scName = satName[index]->begin(); scName != satName[index]->end(); 
           ++scName) 
      {
         #if DEBUG_PROPAGATE_INIT
            MessageInterface::ShowMessage("   Adding '%s' to propsetup '%s'\n",
               scName->c_str(), i->c_str());
         #endif
         if ((mapObj = FindObject(*scName)) == NULL) 
         {
            std::string errmsg = "Unknown SpaceObject \"";
            errmsg += *scName;
            errmsg += "\"";
            throw CommandException(errmsg);
         }
         so = (SpaceObject*)mapObj;
         if (epochID == -1)
            epochID = so->GetParameterID("A1Epoch");
         if (so->IsManeuvering())
            finiteBurnActive = true;
         sats.push_back(so);
         AddToBuffer(so);
         fm->AddSpaceObject(so);
         if (so->GetType() == Gmat::FORMATION)
            FillFormation(so, owners, elements);
         else 
         {
            SetNames(so->GetName(), owners, elements);
         }
      }
      
      // Check for finite thrusts and update the force model if there are any
      if (finiteBurnActive == true)
         AddTransientForce(satName[index], fm);
      
      streamID = publisher->RegisterPublishedData(owners, elements);
      p->SetPhysicalModel(fm);
      p->SetRealParameter("InitialStepSize", 
         fabs(p->GetRealParameter("InitialStepSize")) * direction);
      p->Initialize();
      ++index;
   } // End of loop through PropSetups
   
   initialized = true;

   stopSats.clear();
   // Setup spacecraft array used for stopping conditions
   for (StringArray::iterator sc = stopSatNames.begin(); 
        sc != stopSatNames.end(); ++sc) {

      if ((mapObj = FindObject(*sc)) == NULL)
      {
         std::string errmsg = "Unknown SpaceObject \"";
         errmsg += *sc;
         errmsg += "\" used in stopping conditions";
         throw CommandException(errmsg);
      }
      so = (SpaceObject*)mapObj;
      stopSats.push_back(so);
   }

   #if DEBUG_PROPAGATE_INIT
      for (UnsignedInt i=0; i<stopSats.size(); i++)
         MessageInterface::ShowMessage(
            "Propagate::Initialize() stopSats[%d]=%s\n", i, 
            stopSats[i]->GetName().c_str());
   #endif
   
   if ((stopWhen.size() == 0) && !singleStepMode)
      throw CommandException("No stopping conditions specified!");
   
   if (solarSys != NULL)
   {
      StringArray refNames;
      
      for (UnsignedInt i=0; i<stopWhen.size(); i++)
      {
         stopWhen[i]->SetSolarSystem(solarSys);
         
         //Set StopCondition parameters
         refNames = stopWhen[i]->GetRefObjectNameArray(Gmat::PARAMETER);
         
         for (UnsignedInt j=0; j<refNames.size(); j++)
         {
            #if DEBUG_PROPAGATE_INIT
               MessageInterface::ShowMessage("===> refNames=<%s>\n", 
                  refNames[j].c_str());
            #endif
            mapObj = FindObject(refNames[j]);
            stopWhen[i]->SetRefObject(mapObj,
                                      Gmat::PARAMETER, refNames[j]);
         }
         
         stopWhen[i]->Initialize();
         stopWhen[i]->SetSpacecraft(sats[0]);
         
         if (!stopWhen[i]->IsInitialized())
         {
            initialized = false;
            MessageInterface::ShowMessage(
               "Propagate::Initialize() StopCondition %s is not initialized.\n",
               stopWhen[i]->GetName().c_str());
            break;
         }
      }
   }
   else
   {
      initialized = false;
      MessageInterface::ShowMessage
         ("Propagate::Initialize() SolarSystem not set in StopCondition");
   }

   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage("Propagate::Initialize() complete.\n");
   #endif

   #ifdef DEBUG_PROPAGATE_DIRECTION
      MessageInterface::ShowMessage("Propagate::Initialize():"
                                    " Propagators Identified:\n");
      for (StringArray::iterator i = propName.begin(); i != propName.end();
           ++i)
         MessageInterface::ShowMessage("   \"%s\" running %s\n", i->c_str(),
         (direction > 0.0 ? "forwards" : "backwards"));
   #endif

   if (singleStepMode)
   {
      commandSummary = "Command Summary: ";
      commandSummary += typeName;
      commandSummary += " Command\nSummary not available in single step mode\n";
   }

   #ifdef DUMP_PLANET_DATA
      if (body[0] == NULL)
         body[0] = solarSys->GetBody("Earth");
      if (body[1] == NULL)
         body[1] = solarSys->GetBody("Sun");
      if (body[2] == NULL)
         body[2] = solarSys->GetBody("Luna");
      if (body[3] == NULL)
         body[3] = solarSys->GetBody("Mercury");
      if (body[4] == NULL)
         body[4] = solarSys->GetBody("Venus");
      if (body[5] == NULL)
         body[5] = solarSys->GetBody("Mars");
      if (body[6] == NULL)
         body[6] = solarSys->GetBody("Jupiter");
      if (body[7] == NULL)
         body[7] = solarSys->GetBody("Saturn");
      if (body[8] == NULL)
         body[8] = solarSys->GetBody("Uranus");
      if (body[9] == NULL)
         body[9] = solarSys->GetBody("Neptune");
      if (body[10] == NULL)
         body[10] = solarSys->GetBody("Pluto");
   
      bodiesDefined = 11;      
   #endif

   return initialized;
}

//------------------------------------------------------------------------------
// void FillFormation(SpaceObject *so)
//------------------------------------------------------------------------------
/**
 * Fill in the components of a formation (recursively).
 *
 * @param <so> The SpaceObject that needs to be filled.
 */
//------------------------------------------------------------------------------
void Propagate::FillFormation(SpaceObject *so, StringArray& owners, 
                              StringArray& elements)
{
   GmatBase *mapObj = NULL;
   static Integer soEpochId = -1;
   if ((so == NULL) || (so->GetType() != Gmat::FORMATION))
      throw CommandException("Invalid SpaceObject passed to FillFormation");
   
   if (soEpochId == -1)
      soEpochId = so->GetParameterID("A1Epoch");
      
   StringArray comps = so->GetStringArrayParameter(so->GetParameterID("Add"));
   SpaceObject *el;
   Real ep;
   
   for (StringArray::iterator i = comps.begin(); i != comps.end(); ++i) 
   {
      if ((mapObj = FindObject(*i)) == NULL)
         throw CommandException("Formation " + so->GetName() +
            " uses unknown object named '" + (*i) + "'");
            
      el = (SpaceObject*)mapObj;
      if (i == comps.begin())
      {
         ep = el->GetRealParameter(soEpochId);
         so->SetRealParameter(soEpochId, ep);
      }
      
      so->SetRefObject(el, el->GetType(), el->GetName()); 
      if (el->GetType() == Gmat::FORMATION)
         FillFormation(el, owners, elements);
      else     // Setup spacecraft data descriptions
         SetNames(el->GetName(), owners, elements);
   }
   
   ((Formation*)(so))->BuildState();
}


//------------------------------------------------------------------------------
// GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Returns pointer to next command to be executed.
 * 
 * Propagate::GetNext overrides the base class's GetNext method so that it can
 * poll the moderator for user interrupts periodically.  If the stopping 
 * conditions have not yet been met, GetNext returns this object; otherwise, it 
 * returns the next one in the command list.
 *
 * @return The next pointer, as described above.
 */
//------------------------------------------------------------------------------
GmatCommand* Propagate::GetNext()
{
   if (!inProgress)
      return next;
   return this;
}

//------------------------------------------------------------------------------
// void PrepareToPropagate()
//------------------------------------------------------------------------------
/**
 * Performs initialization needed immediately before propagating.
 */
//------------------------------------------------------------------------------
void Propagate::PrepareToPropagate()
{
   #ifdef DEBUG_PROP_PERFORMANCE
      MessageInterface::ShowMessage(
         "Entered PrepareToPropagate; hasFired = %s\n", 
         (hasFired ? "True" : "False"));
   #endif
   
   if (hasFired == true) 
   {
      // Handle the transient forces
      for (std::vector<SpaceObject *>::iterator sc = sats.begin(); 
           sc != sats.end(); ++sc)
      {
         if ((*sc)->IsManeuvering())
         {
            #ifdef DEBUG_FINITE_MANEUVER
               MessageInterface::ShowMessage(
                  "SpaceObject %s is maneuvering\n", (*sc)->GetName().c_str());
            #endif
            
            // Add the force
            for (UnsignedInt index = 0; index < prop.size(); ++index)
            {
               for (std::vector<PhysicalModel*>::iterator i = transientForces->begin();
                    i != transientForces->end(); ++i) 
               {
                  ForceModel *fm = prop[index]->GetForceModel();
                  const StringArray sar = fm->GetRefObjectNameArray(Gmat::SPACEOBJECT);
                  if (find(sar.begin(), sar.end(), (*sc)->GetName()) != sar.end()) 
                     prop[index]->GetForceModel()->AddForce(*i);
               }
            }
         }
      }

      for (Integer n = 0; n < (Integer)prop.size(); ++n)
      {
         elapsedTime[n] = 0.0;
         currEpoch[n]   = 0.0;
         fm[n]->SetTime(0.0);
         fm[n]->UpdateInitialData();
      
         p[n]->Initialize();
         p[n]->Update(direction > 0.0);
         state = fm[n]->GetState();
         j2kState = fm[n]->GetJ2KState();
      }   

      baseEpoch.clear();

      for (Integer n = 0; n < (Integer)prop.size(); ++n) {
         #if DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage
               ("Propagate::PrepareToPropagate() SpaceObject names\n");
            
            MessageInterface::ShowMessage
               ("SpaceObject Count = %d\n", satName[n]->size());
            StringArray *sar = satName[n];
            for (Integer i=0; i < (Integer)satName[n]->size(); i++)
            {
               MessageInterface::ShowMessage
                  ("   SpaceObjectName[%d] = %s\n", i, (*sar)[i].c_str());
            }
         #endif
         
         if (satName[n]->empty())
            throw CommandException(
               "Propagator has no associated space objects.");

         GmatBase* sat1 = FindObject(*satName[n]->begin());
         baseEpoch.push_back(sat1->GetRealParameter(epochID));
         elapsedTime[n] = fm[n]->GetTime();
         currEpoch[n] = baseEpoch[n] + elapsedTime[n] /
            GmatTimeUtil::SECS_PER_DAY;
         #if DEBUG_PROPAGATE_DIRECTION
            MessageInterface::ShowMessage(
               "Propagate::PrepareToPropagate() running %s %s.\n",
               prop[n]->GetName().c_str(),
               (prop[n]->GetPropagator()->GetRealParameter("InitialStepSize") > 0.0
                  ? "forwards" : "backwards"));
             MessageInterface::ShowMessage("   direction =  %lf.\n",
               direction);
         #endif
      }
   
      // Now setup the stopping condition elements
      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage
            ("Propagate::PrepareToPropagate() Propagate start; epoch = %f\n",
          (baseEpoch[0] + fm[0]->GetTime() / GmatTimeUtil::SECS_PER_DAY));
         MessageInterface::ShowMessage
            ("Propagate::PrepareToPropagate() Propagate start; fm epoch = %f\n",
            (fm[0]->GetRealParameter(fm[0]->GetParameterID("Epoch"))));
         Integer stopCondCount = stopWhen.size();
         MessageInterface::ShowMessage
            ("Propagate::PrepareToPropagate() stopCondCount = %d\n", stopCondCount);
            
         for (Integer i=0; i<stopCondCount; i++)
         {
            MessageInterface::ShowMessage
               ("Propagate::PrepareToPropagate() stopCondName[%d]=%s\n", i,
                      stopWhen[i]->GetName().c_str());
         }
      #endif
       
      stopCondMet = false;
      stopEpoch = 0.0;
      std::string stopVar;
      Real stopEpochBase;
      
      #ifdef DEBUG_STOPPING_CONDITIONS
         if (!singleStepMode)
            MessageInterface::ShowMessage(
               "Stopping condition IDs are [%d, %d, %d]\n",
               stopCondEpochID, stopCondBaseEpochID, stopCondStopVarID);
      #endif
      
      
      try {
         for (UnsignedInt i = 0; i<stopWhen.size(); i++)
         {
            if (i >= stopSats.size())
               throw CommandException("Stopping condition " + 
               stopWhen[i]->GetName() + " has no associated spacecraft.");
      
            #if DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage(
                  "Propagate::PrepareToPropagate() stopSat = %s\n",
                  stopSats[i]->GetName().c_str());
            #endif
      
            stopEpochBase = stopSats[i]->GetRealParameter(epochID);
            
            // StopCondition need new base epoch
            stopWhen[i]->SetRealParameter(stopCondBaseEpochID, stopEpochBase);
      
            // ElapsedTime parameters need new initial epoch
            stopVar = stopWhen[i]->GetStringParameter(stopCondStopVarID);
            if (stopVar.find("Elapsed") != stopVar.npos)
            {
               stopWhen[i]->GetStopParameter()->
                  SetRealParameter("InitialEpoch", stopEpochBase);
            }
         }
      }
      catch (BaseException &ex) {
         MessageInterface::ShowMessage(
            "Propagate::PrepareToPropagate() Exception while initializing stopping "
            "conditions\n");
         inProgress = false;
         throw;
      }

      // Publish the initial data
      pubdata[0] = currEpoch[0];
      memcpy(&pubdata[1], j2kState, dim*sizeof(Real));
      publisher->Publish(streamID, pubdata, dim+1);

      inProgress = true;
      return;
   }

   // Reset the initialization data
   Initialize();

   // Validate that all spacecraft coupled by a PropSetup have same epoch
   // Comment this for loop to skip epoch matching checks
   for (Integer n = 0; n < (Integer)prop.size(); ++n) {
      StringArray *sar = satName[n];
      GmatBase* sat1;
      Real baseEp = 0.0;
      for (StringArray::iterator s = sar->begin(); s != sar->end(); ++s)
      {
         sat1 = FindObject(*s);
         if (s == sar->begin())
            baseEp = sat1->GetRealParameter(epochID);
         else if (baseEp != sat1->GetRealParameter(epochID))
         {
            #ifdef DEBUG_EPOCH_SYNC
               MessageInterface::ShowMessage(
                     "Epoch mismatch:   Base = %18.12lf\n   %s = %18.12lf\n"
                     "   delta = %18.12le\n\n", baseEp, sat1->GetName().c_str(), 
                     sat1->GetRealParameter(epochID),
                     (sat1->GetRealParameter(epochID) - baseEp));
               for (StringArray::iterator sd = sar->begin(); sd != sar->end(); ++sd)
               {
                  GmatBase *theSat = FindObject(*sd);
                  MessageInterface::ShowMessage("   %s epoch = %18.12lf\n", 
                        theSat->GetName().c_str(), 
                        theSat->GetRealParameter(epochID));
               }
            #endif
            throw CommandException(
               "Epochs are out of sync on Propagation line:\n\"" + 
               generatingString + "\"\n");
         }
      }
   }
   
   dim = 0;
   p.clear();
   fm.clear();
   
   for (Integer n = 0; n < (Integer)prop.size(); ++n) {
      elapsedTime.push_back(0.0);
      currEpoch.push_back(0.0);
      p.push_back(prop[n]->GetPropagator());
      fm.push_back(prop[n]->GetForceModel());
      fm[n]->SetTime(0.0);
      fm[n]->UpdateInitialData();
   
      p[n]->Initialize();
      p[n]->Update(direction > 0.0);
      state = fm[n]->GetState();
      j2kState = fm[n]->GetJ2KState();
      dim += fm[n]->GetDimension();
   }   

   pubdata = new Real[dim+1];
   baseEpoch.clear();
   
   for (Integer n = 0; n < (Integer)prop.size(); ++n) {
      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage
            ("Propagate::PrepareToPropagate() SpaceObject names\n");
         
         MessageInterface::ShowMessage
            ("SpaceObject Count = %d\n", satName[n]->size());
         StringArray *sar = satName[n];
         for (Integer i=0; i < (Integer)satName[n]->size(); i++)
         {
            MessageInterface::ShowMessage
               ("   SpaceObjectName[%d] = %s\n", i, (*sar)[i].c_str());
         }
      #endif
      
      if (satName[n]->empty())
         throw CommandException(
            "Propagator has no associated space objects.");
      GmatBase* sat1 = FindObject(*(satName[n]->begin()));

      baseEpoch.push_back(sat1->GetRealParameter(epochID));
      elapsedTime[n] = fm[n]->GetTime();
      currEpoch[n] = baseEpoch[n] + elapsedTime[n] /
         GmatTimeUtil::SECS_PER_DAY;
      #if DEBUG_PROPAGATE_DIRECTION
         MessageInterface::ShowMessage(
            "Propagate::PrepareToPropagate() running %s %s.\n",
            prop[n]->GetName().c_str(),
            (prop[n]->GetPropagator()->GetRealParameter("InitialStepSize") > 0.0
               ? "forwards" : "backwards"));
          MessageInterface::ShowMessage("   direction =  %lf.\n",
            direction);
      #endif
   }

   // Now setup the stopping condition elements
   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::PrepareToPropagate() Propagate start; epoch = %f\n",
       (baseEpoch[0] + fm[0]->GetTime() / GmatTimeUtil::SECS_PER_DAY));
      MessageInterface::ShowMessage
         ("Propagate::PrepareToPropagate() Propagate start; fm epoch = %f\n",
         (fm[0]->GetRealParameter(fm[0]->GetParameterID("Epoch"))));
      Integer stopCondCount = stopWhen.size();
      MessageInterface::ShowMessage
         ("Propagate::PrepareToPropagate() stopCondCount = %d\n", stopCondCount);
      for (Integer i=0; i<stopCondCount; i++)
      {
         MessageInterface::ShowMessage
            ("Propagate::PrepareToPropagate() stopCondName[%d]=%s\n", i,
                   stopWhen[i]->GetName().c_str());
      }
   #endif
    
   stopCondMet = false;
   stopEpoch = 0.0;
   std::string stopVar;
   Real stopEpochBase;
   
   try {
      for (UnsignedInt i = 0; i<stopWhen.size(); i++)
      {
         if (i >= stopSats.size())
            throw CommandException("Stopping condition " + 
            stopWhen[i]->GetName() + " has no associated spacecraft.");
   
         #if DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage(
               "Propagate::PrepareToPropagate() stopSat = %s\n",
               stopSats[i]->GetName().c_str());
         #endif
   
         stopEpochBase = stopSats[i]->GetRealParameter(epochID);
         
         // StopCondition need new base epoch
         stopWhen[i]->SetRealParameter(stopCondBaseEpochID, stopEpochBase);
   
         // ElapsedTime parameters need new initial epoch
         stopVar = stopWhen[i]->GetStringParameter(stopCondStopVarID);
         if (stopVar.find("Elapsed") != stopVar.npos)
         {
            stopWhen[i]->GetStopParameter()->
               SetRealParameter("InitialEpoch", stopEpochBase);
         }
      }
   }
   catch (BaseException &ex) {
      MessageInterface::ShowMessage(
         "Propagate::PrepareToPropagate() Exception while initializing stopping "
         "conditions\n");
      inProgress = false;
      throw;
   }
   
   // Publish the initial data
   pubdata[0] = currEpoch[0];
   memcpy(&pubdata[1], j2kState, dim*sizeof(Real));
   publisher->Publish(streamID, pubdata, dim+1);

   hasFired = true;
   inProgress = true;

   #ifdef DEBUG_FIRST_CALL
   if (state)
   {
      MessageInterface::ShowMessage("Debugging first step\n");
      MessageInterface::ShowMessage(
         "State = [%16.9lf %16.9lf %16.9lf %16.14lf %16.14lf %16.14lf]\n",
         state[0], state[1], state[2], state[3], state[4], state[5]);
      MessageInterface::ShowMessage(
         "Propagator = \n%s\n", 
         prop[0]->GetGeneratingString(Gmat::SCRIPTING, "   ").c_str());
   }
   else
      MessageInterface::ShowMessage("Debugging first step: State not set\n");
      firstStepFired = true;
   #endif
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Propagate the assigned members to the desired stopping condition
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Propagate::Execute()
{
   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage("Propagate::Execute() <%s> entered.\n",
                                    GetGeneratingString().c_str());
   #endif

   if (initialized == false)
      throw CommandException("Propagate Command was not Initialized\n");

   // Parm used to check for interrupt in the propagation   
   Integer checkCount = 0, trigger = 0;

   try 
   {
      // If command is not reentering from checking interrupts, do final prep
      if (!inProgress)
      {
         stepBrackets[0] = 0.0;
         checkFirstStep = false;
         PrepareToPropagate();

         // Check for initial stop condition before first step in while loop
         // eg) elapsed time of 0
         if (publisher->GetRunState() == Gmat::RUNNING)
         {
            // remove any old stop conditions that may have reported valid
            triggers.clear();
            stopTrigger = -1;
            // Evaluate Stop conditions to set initial values
            for (UnsignedInt i=0; i<stopWhen.size(); i++)
            {
               Real accuracy = (stopWhen[i]->IsTimeCondition() ? timeAccuracy : 
                                firstStepTolerance);
               stopWhen[i]->Reset();
               stopWhen[i]->Evaluate();

               // Set the flag to check the first step only if 
               //    (1) the stop value is <= stopAccuracy and
               //    (2) it was (one of) the last stop(s) triggered
               if (fabs(stopWhen[i]->GetStopValue() - stopWhen[i]->GetStopGoal()) < accuracy)
               {
                  #ifdef DEBUG_FIRST_STEP_STOP
                     std::string scName = stopWhen[i]->GetName();
                     MessageInterface::ShowMessage(
                        "*** FirstStep: Value = %.12lf, goal = %.12lf\n*** "
                        "Name = %s, WLST = %s\n", stopWhen[i]->GetStopValue(), 
                        stopWhen[i]->GetStopGoal(), 
                        scName.c_str(), 
                        (stopSats[i]->WasLastStopTriggered(scName) ? 
                         "true" : "false"));
                  #endif
   
                  if (stopSats[i]->WasLastStopTriggered(stopWhen[i]->GetName()))
                  {
                     checkFirstStep = true;
                     stopWhen[i]->SkipEvaluation(true);
                  }
               }
            }
         }
      }
      
      #ifdef DEBUG_EPOCH_SYNC
         MessageInterface::ShowMessage("Nominal steps executing:\n");
      #endif
      
      while (!stopCondMet)
      {
         // Update the epoch on the force models
         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            fm[i]->UpdateInitialData();
         }
         #ifdef DEBUG_EPOCH_UPDATES
            fm[0]->ReportEpochData();
         #endif

         if (!TakeAStep())
            throw CommandException(
               "Propagate::Execute() Propagator Failed to Step\n");
         for (UnsignedInt i = 0; i < fm.size(); ++i) {
            // orbit related parameters use spacecraft for data
            elapsedTime[i] = fm[i]->GetTime();
            currEpoch[i] = baseEpoch[i] + elapsedTime[i] /
               GmatTimeUtil::SECS_PER_DAY;
            
            // Update spacecraft epoch, without argument the spacecraft epoch
            // won't get updated for consecutive Propagate command
            fm[i]->UpdateSpaceObject(currEpoch[i]);
         }

         // In single step mode, we're done!
         if (singleStepMode)
            break;

         CheckStopConditions(epochID);
         
         if (!stopCondMet)
         {
            // No longer need to check stopping conditions at the first step
            checkFirstStep = false;
            
            // Publish the data here
            pubdata[0] = currEpoch[0];
            memcpy(&pubdata[1], j2kState, dim*sizeof(Real));
            publisher->Publish(streamID, pubdata, dim+1);
         }
         else
         {  
            stopInterval = 0.0;
            for (UnsignedInt i = 0; i < fm.size(); ++i) 
            {
               Real timestep = p[i]->GetStepTaken();
               if (fabs(timestep) > fabs(stopInterval))
                  stopInterval = timestep;
               
               fm[i]->RevertSpaceObject();
               switch (currentMode)
               {
                  case SYNCHRONIZED:
                     elapsedTime[i] = fm[0]->GetTime();
                     fm[i]->SetTime(elapsedTime[i]);
                     break;
                     
                  case INDEPENDENT:
                  default:
                     elapsedTime[i] = fm[i]->GetTime();
               }
               
               currEpoch[i] = baseEpoch[i] +
                  elapsedTime[i] / GmatTimeUtil::SECS_PER_DAY;
            }

            stepBrackets[1] = stopInterval;

            #ifdef DEBUG_EPOCH_SYNC
               for (UnsignedInt i = 0; i < fm.size(); ++i) 
                  MessageInterface::ShowMessage(
                     "   SC MET!  Force model[%d] has base epoch %16.11lf, "
                     "time dt = %.11lf, elapsedTime = %.11lf\n", i, 
                     baseEpoch[i], fm[i]->GetTime(), elapsedTime[i]);
            #endif
            
            #ifdef DEBUG_EPOCH_UPDATES
               MessageInterface::ShowMessage("StopStep = %15.11lf\n", 
                  stopInterval);
            #endif
         }
         
         #if DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage(
               "Propagate::Execute() intermediate; epoch = %f\n", currEpoch[0]);
         #endif
   
         // Periodically see if the user has stopped the run 
         ++checkCount;
         if ((checkCount == interruptCheckFrequency) && !stopCondMet)
         {
            inProgress = true;
            return true;
         }

         #ifdef DEBUG_EPOCH_SYNC
            for (UnsignedInt i = 0; i < fm.size(); ++i) 
               MessageInterface::ShowMessage("   Force model[%d] has base "
                  "epoch %16.11lf, time dt = %.11lf\n",
                  i, baseEpoch[i], fm[i]->GetTime());
         #endif
      }
         #ifdef DEBUG_EPOCH_SYNC
            MessageInterface::ShowMessage("Nominal steps Finished\n");
         #endif
   }
   catch (BaseException &ex) 
   {
      inProgress = false;
      throw;
   }

   #ifdef DEBUG_EPOCH_UPDATES
      fm[0]->ReportEpochData();
   #endif

   inProgress = false;
   if (!singleStepMode)
   {
      TakeFinalStep(epochID, trigger);
      // reset the stopping conditions so that scanning starts over
      for (UnsignedInt i=0; i<stopWhen.size(); i++)
         stopWhen[i]->Reset(); 
   }
   else
   {
      // clear first step stopping condition flags
      for (std::vector<SpaceObject *>::iterator i = sats.begin(); 
           i != sats.end(); ++i)
         (*i)->ClearLastStopTriggered();
   }

   #ifdef DEBUG_EPOCH_UPDATES
      fm[0]->ReportEpochData();
   #endif
   
   ClearTransientForces();
   // Only build command summary if not in single step mode
   if (!singleStepMode)
      BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
// bool TakeAStep(Real propStep)
//------------------------------------------------------------------------------
/**
 * Advances each of the contained PropSetups by one step.
 *
 * @param <propStep> The requested size of the step.
 *
 * @return true if the step succeeded.
 */
//------------------------------------------------------------------------------
bool Propagate::TakeAStep(Real propStep)
{
   bool retval = false;
   Real stepToTake;
 
   #ifdef DEBUG_FIXED_STEP
      std::vector<ForceModel *>::iterator fmod = fm.begin();
   #endif

   std::vector<Propagator*>::iterator current = p.begin();
   if (propStep == 0.0) 
   {
      switch (currentMode) 
      {
         case INDEPENDENT:
            // Advance each propagator individually, without regard for the
            // epochs of the others
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() running in INDEPENDENT mode\n");
            #endif
            while (current != p.end()) 
            { 
               if (!(*current)->Step())
                  throw CommandException(
                     "Propagator failed to take a good step\n");
               ++current;
            }
            retval = true;
            break;
            
         case SYNCHRONIZED:
            // This mode advances the first propagator, and then brings the 
            // others up to the epoch of that first one.
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() running in SYNCHRONIZED mode\n");
            #endif
            if (!(*current)->Step())
               throw CommandException("Initial synchronized Propagator failed "
                                      "to take a good step\n");
            stepToTake = (*current)->GetStepTaken();
            ++current;
            while (current != p.end()) 
            {
               if (!(*current)->Step(stepToTake))
                  throw CommandException("Propagator failed to take a good "
                                         "synchronized step\n");
               ++current;
            }
            retval = true;
            break;
            
         default:
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() runnning in undefined mode "
                  "(mode = %d)\n", currentMode);
            #endif
            retval = false;
      }
   }
   else 
   {
      // Step all of the propagators by the input amount
      while (current != p.end()) 
      {
         #ifdef DEBUG_FIXED_STEP
            MessageInterface::ShowMessage("Stepping '%s' by %le seconds from "
               "epoch = %16.11lf\n", (*current)->GetName().c_str(), propStep,
               (*fmod)->GetRealParameter((*fmod)->GetParameterID("Epoch")));
 
            Integer fmSize = (*fmod)->GetDimension();
            MessageInterface::ShowMessage("Fmod has dim = %d\n", fmSize);
            MessageInterface::ShowMessage("   Pre Prop:  ");
           
            Real *fmState = (*fmod)->GetState();
            for (Integer q = 0; q < fmSize; ++q)
               MessageInterface::ShowMessage(" %.12lf", fmState[q]);
            MessageInterface::ShowMessage("\n");
         #endif

         if (!(*current)->Step(propStep))
         {
            char size[32];
            std::sprintf(size, "%.12lf", propStep);
            throw CommandException("Propagator " + (*current)->GetName() + 
               " failed to take a good final step (size = " + size + ")\n");
         }
         

         #ifdef DEBUG_FIXED_STEP
            MessageInterface::ShowMessage("   Stepped to epoch = %16.11lf\n",
               (*fmod)->GetRealParameter((*fmod)->GetParameterID("Epoch")));
            MessageInterface::ShowMessage("   Post Prop: ");
           
            for (Integer q = 0; q < fmSize; ++q)
               MessageInterface::ShowMessage(" %.12lf", fmState[q]);
            MessageInterface::ShowMessage("\n");
            
            ++fmod;
         #endif
         
         ++current;
      }
      retval = true;
   }
   
   #ifdef DEBUG_PROPAGATE_STEPSIZE
      MessageInterface::ShowMessage("Prop step = %16.13lf\n", 
         p[0]->GetStepTaken());
   #endif
   
   #ifdef DUMP_PLANET_DATA
      Real epoch = sats[0]->GetRealParameter("A1Epoch");
      Rvector6 planetrv;
      
      for (Integer h = 0; h < bodiesDefined; ++h)
      {
         if (body[h] != NULL)
         {
            planetrv = body[h]->GetState(epoch);
            planetData << (body[h]->GetName()) << "  " << epoch << "  " 
                       << planetrv[0] << "  " 
                       << planetrv[1] << "  " 
                       << planetrv[2] << "  " 
                       << planetrv[3] << "  " 
                       << planetrv[4] << "  " 
                       << planetrv[5] << "\n";
         }
      }
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void CheckStopConditions(Integer epochID)
//------------------------------------------------------------------------------
/**
 * Checks the status of the stopping conditions.
 * 
 * @param epochID The parameter ID associated with the epoch field.
 */
//------------------------------------------------------------------------------
void Propagate::CheckStopConditions(Integer epochID)
{
   //------------------------------------------
   // loop through StopCondition list
   //------------------------------------------
   #ifdef DEBUG_STOPPING_CONDITIONS
      try {
   #endif
   
      for (UnsignedInt i = 0; i < stopWhen.size(); i++)
      {
         // StopCondition need epoch for the Interpolator
         stopWhen[i]->SetRealParameter(stopCondEpochID,
            stopSats[i]->GetRealParameter(epochID));
         
         #ifdef DEBUG_STOPPING_CONDITIONS
            MessageInterface::ShowMessage(
               "Evaluating \"%s\" Stopping condition\n",
               stopWhen[i]->GetName().c_str());
         #endif
         
         if (stopWhen[i]->Evaluate())
         {
            stopInterval = stopWhen[i]->GetStopInterval();
            if (stopInterval == 0.0)
            {
               stopEpoch = stopWhen[i]->GetStopEpoch();
            }
            
            stopCondMet = true;
            if (stopTrigger < 0)
               stopTrigger = i;
                        
            triggers.push_back(stopWhen[i]);
   
            #if DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::CheckStopConditions() %s met for %d\n", 
                   stopWhen[i]->GetName().c_str(), i);
            #endif

            #ifdef DEBUG_EPOCH_SYNC
               for (UnsignedInt i = 0; i < fm.size(); ++i) 
                  MessageInterface::ShowMessage("   *** SC[%d] (%s) MET! ***\n", 
                     i, stopWhen[i]->GetName().c_str());
            #endif
         }
         else if (checkFirstStep)
         {
            #ifdef DEBUG_FIRST_STEP_STOP
               MessageInterface::ShowMessage("%d: ", i);
            #endif

            // Turn condition back on
            stopWhen[i]->SkipEvaluation(false);
            if (CheckFirstStepStop(i))
            {
               #ifdef DEBUG_FIRST_STEP_STOP
                  MessageInterface::ShowMessage("***Continuing from %s\n", 
                     stopWhen[i]->GetName().c_str());
               #endif
               
               stopInterval = stopWhen[i]->GetStopInterval();
               if (stopInterval == 0.0)
               {
                  stopEpoch = stopWhen[i]->GetStopEpoch();
               }
               stopCondMet = true;
               if (stopTrigger < 0)
                  stopTrigger = i;
                           
               triggers.push_back(stopWhen[i]);
            }
         }
      }
      
   #ifdef DEBUG_STOPPING_CONDITIONS
      }
      catch (BaseException &ex) {
         MessageInterface::ShowMessage(
            "Propagate::PrepareToPropagate() Exception while evaluating stopping "
            "conditions\n");
         inProgress = false;
         throw;
      }
   #endif

   #ifdef DEBUG_EPOCH_SYNC
      for (UnsignedInt i = 0; i < stopWhen.size(); ++i)
         if (stopWhen[i]->Evaluate())
            MessageInterface::ShowMessage("   *** StopInterval[%d] = %.11lf\n", 
               i, stopWhen[i]->GetStopInterval());
   #endif
}


//------------------------------------------------------------------------------
// bool CheckFirstStep()
//------------------------------------------------------------------------------
/**
 * Method used during the first prop step to ensure that a stop encountered on 
 * this step is not repeating the last stop encountered.
 * 
 * @param <i> Index in the stopping condition list that needs to be checked
 * 
 * @return true is the stop is a valid stop -- that is, if it is not a repeat of
 * the last stop -- and false if it is a repeat.
 * 
 * @note: Not yet implemented; the method always returns true for now.
 */
//------------------------------------------------------------------------------
bool Propagate::CheckFirstStepStop(Integer i)
{
   #ifdef DEBUG_FIRST_STEP_STOP
      MessageInterface::ShowMessage(
         "CheckFirstStepStop checking %s; returning valid stop condition: %s\n", 
         stopWhen[i]->GetName().c_str(),
         (stopSats[i]->WasLastStopTriggered(stopWhen[i]->GetName()) ?
          "false" : "true"));
   #endif
   
   if (stopSats[i]->WasLastStopTriggered(stopWhen[i]->GetName()))//;
   {
      // Only report as triggered if outside of the stop accuracy
      Real min, max, goal, temp;
      min = stopWhen[i]->GetStopValue();
      max = stopWhen[i]->GetStopParameter()->EvaluateReal();
      if (min > max)
      {
         temp = min;
         min = max;
         max = temp;
      }
      goal = stopWhen[i]->GetStopGoal(); 
      
      temp = fabs(goal - min);
      if (fabs(goal - max) < temp)
         temp = fabs(goal - max);
      
      // Only report true if outside of tolerance
      Real accuracy = (stopWhen[i]->IsTimeCondition() ? timeAccuracy : 
                                                        firstStepTolerance);

      if (temp > accuracy)
         if ((goal > min) && (goal < max))
            return true;
      
      // Fill buffer data in the sc
      stopWhen[i]->UpdateBuffer();
   }
      
   return false;
}


//------------------------------------------------------------------------------
// void TakeFinalStep(Integer EpochID, Integer trigger)
//------------------------------------------------------------------------------
/**
 * Takes the final prop step based on data from the stopping conditions.
 * 
 * @param epochID The parameter ID associated with the epoch field.
 * @param trigger Index indicating which stopping condition was met.
 */
//------------------------------------------------------------------------------
void Propagate::TakeFinalStep(Integer EpochID, Integer trigger)
{
   // We've passed a stop condition, so remember that step size.  Include a 10%
   // safety factor.
   stepBrackets[1] = stopInterval * 1.1;

   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage(
         "Propagate::TakeFinalStep currEpoch = %f, stopEpoch = %f, "
         "elapsedTime = %f\n", currEpoch[0], stopEpoch, elapsedTime[0]);
   #endif

   Real secsToStep = 1.0e99 * direction, dt;
   StopCondition *stopper = NULL;

   #ifdef DEBUG_EPOCH_SYNC
      MessageInterface::ShowMessage("Top of final step code:\n");
      for (UnsignedInt i = 0; i < fm.size(); ++i) 
         MessageInterface::ShowMessage(
            "   Force model[%d] has base epoch %16.11lf, time dt = %.11lf\n",
            i, baseEpoch[i], fm[i]->GetTime());
   #endif   
   
   // Toggle propagators into final step mode
   /// @note This code should be removed if the minimum step is removed from 
   /// the RK integrators
   for (std::vector<Propagator*>::iterator current = p.begin(); 
        current != p.end(); ++current)
      (*current)->SetAsFinalStep(true);

   // First save the spacecraft for later restoration
   for (UnsignedInt i = 0; i < fm.size(); ++i) 
   {
      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage("   CurrentEpoch[%d] = %.12lf\n", i,
            currEpoch[i]);
      #endif
      fm[i]->UpdateSpaceObject(currEpoch[i]);
   }
   BufferSatelliteStates(true);
   
   #ifdef DEBUG_EPOCH_SYNC
      MessageInterface::ShowMessage("Prior to interpolation:\n");
      for (UnsignedInt i = 0; i < fm.size(); ++i) 
         MessageInterface::ShowMessage(
            "   Force model[%d] has base epoch %16.11lf, time dt = %.11lf\n",
            i, baseEpoch[i], fm[i]->GetTime());
   #endif
   
   // Interpolate to get the stop epoch
   if (stopTrigger < 0)
      throw CommandException(
         "Stopping condition was not set for final step on the line \n" +
         GetGeneratingString(Gmat::SCRIPTING));

   for (std::vector<StopCondition*>::iterator i = triggers.begin(); 
        i != triggers.end(); ++i)
   {
      // Get estimated time to reach this stop condition, dt
      if ((*i)->IsTimeCondition())
      {
         dt = (*i)->GetStopEpoch();
                  
         #ifdef DEBUG_PROPAGATE_STEPSIZE         
            MessageInterface::ShowMessage("Stopping on time\n   current "
               "epoch = %.14lf\n   goal          = %.14lf\n   dt            = "
               "%.14lf\n   sc diff       = %.14lf\n", currEpoch[0], 
               (*i)->GetStopGoal(), dt, (*i)->GetStopDifference()); 
         #endif
      }
      else
      {
         dt = InterpolateToStop(*i);
         
         #ifdef DEBUG_PROPAGATE_STEPSIZE         
            MessageInterface::ShowMessage(
               "Interpolated stop time = %.14lf\n", dt); 
         #endif      
      }

      // If dt is closer to current epoch, save this stop condition as trigger
      if (fabs(secsToStep) > fabs(dt))
      {
         secsToStep = dt;
         stopper = *i;
      }
   }

   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage(
         "Step = %.12lf sec, calculated off of %.12lf and  %.12lf\n", 
         secsToStep, stopEpoch, currEpoch[trigger]);
   #endif
      
   // Perform stepsize rounding.  Note that the rounding precision can be set
   // by redefining the macro TIME_ROUNDOFF at the top of this file.  Set it to
   // 0.0 to prevent rounding.
   //
   // Note that this code makes the final propagated state match the granularity 
   // given by other software (aka STK)
   if (TIME_ROUNDOFF != 0.0)
      secsToStep = std::floor(secsToStep / TIME_ROUNDOFF + 0.5) * TIME_ROUNDOFF;

   #if defined DEBUG_PROPAGATE_STEPSIZE | defined DEBUG_PROPAGATE_DIRECTION
      MessageInterface::ShowMessage
         ("Propagate::TakeFinalStep secsToStep at stop = %16.10le\n",
          secsToStep);
   #endif
   #ifdef DEBUG_PROPAGATE_DIRECTION
      MessageInterface::ShowMessage
         ("   stopEpoch = %16.10lf\n   currEpoch = %16.10lf\n",
          stopEpoch, currEpoch[trigger]);
   #endif

   Real accuracy = (stopper->IsTimeCondition() ? timeAccuracy : stopAccuracy);

   // If we are not at the final state, move to it
   if (secsToStep != 0.0)
   {
      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage(
            "Propagate::TakeFinalStep: Step(%16.13le) from epoch = %16.10lf\n", 
            secsToStep, 
            (baseEpoch[0] + fm[0]->GetTime() / GmatTimeUtil::SECS_PER_DAY));
      #endif

      #ifdef DEBUG_EPOCH_SYNC
         MessageInterface::ShowMessage("Before final step:\n");
         for (UnsignedInt i = 0; i < fm.size(); ++i) 
            MessageInterface::ShowMessage("   Force model[%d] has base "
               "epoch %16.11lf, time dt = %.11lf\n",
               i, baseEpoch[i], fm[i]->GetTime());
         MessageInterface::ShowMessage("   step by time %.11lf\n", secsToStep);
      #endif
         
      if (!TakeAStep(secsToStep))
         throw CommandException("Propagator Failed to Step fixed interval\n");

      #ifdef DEBUG_EPOCH_SYNC
         MessageInterface::ShowMessage("After final step:\n");
         for (UnsignedInt i = 0; i < fm.size(); ++i) 
            MessageInterface::ShowMessage(
               "   Force model[%d] has base epoch %16.11lf, time dt = %.11lf\n",
               i, baseEpoch[i], fm[i]->GetTime());
      #endif

      // Check the stopping accuracy
      for (UnsignedInt i = 0; i < fm.size(); ++i) 
      {
         fm[i]->UpdateSpaceObject(
            baseEpoch[i] + fm[i]->GetTime() / GmatTimeUtil::SECS_PER_DAY);
      }

      stopper->Evaluate();

      if (fabs(stopper->GetStopDifference()) > accuracy)
      {
         #ifdef DEBUG_STOPPING_CONDITIONS   
            MessageInterface::ShowMessage("   Stop condition %s missed by %le; "
               "refining step \n", stopper->GetName().c_str(), 
               stopper->GetStopDifference());
            MessageInterface::ShowMessage(
               "   When stepping %15.10lf secs, parameter has value %.9le\n",
               secsToStep, stopper->GetStopParameter()->EvaluateReal());
         #endif
         
         // The interpolated step was not close enough, so back it out
         BufferSatelliteStates(false);
         for (UnsignedInt i = 0; i < fm.size(); ++i) 
         {
            fm[i]->UpdateFromSpaceObject();
            // Back out the steps taken to build the ring buffer
            fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
         }

         // Generate a better time step
         secsToStep = RefineFinalStep(secsToStep, stopper);
         
         // Perform stepsize rounding.  Note that the rounding precision can be 
         // set by redefining the macro TIME_ROUNDOFF at the top of this file.  
         // Set it to 0.0 to prevent rounding.
         //
         // Note that this code makes the final propagated state match the 
         // granularity given by other software (aka STK)
         if (TIME_ROUNDOFF != 0.0)
            secsToStep = std::floor(secsToStep / TIME_ROUNDOFF + 0.5) * TIME_ROUNDOFF;
      
         // If a refined step was needed, we still need to take it; 
         // RefineFinalStep returns with the interpolated step backed out
         if (!TakeAStep(secsToStep))
            throw CommandException(
               "Propagator Failed to Step fixed interval\n");

         for (UnsignedInt i = 0; i < fm.size(); ++i) 
         {
            fm[i]->UpdateSpaceObject(
               baseEpoch[i] + fm[i]->GetTime() / GmatTimeUtil::SECS_PER_DAY);
         }

         #ifdef DEBUG_STOPPING_CONDITIONS   
            MessageInterface::ShowMessage(
               "   Refined timestep of %15.10lf secs gives %.9le\n", 
               secsToStep, stopper->GetStopParameter()->EvaluateReal());
         #endif
      }

      // Publish the final data point here
      pubdata[0] = baseEpoch[0] + fm[0]->GetTime() / GmatTimeUtil::SECS_PER_DAY;
      memcpy(&pubdata[1], j2kState, dim*sizeof(Real));
      publisher->Publish(streamID, pubdata, dim+1);
      
      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage
            ("Propagate::TakeFinalStep: Step(%16.13le) advanced to "
            "epoch = %16.10lf\n", secsToStep,
            (baseEpoch[0] + fm[0]->GetTime() / GmatTimeUtil::SECS_PER_DAY));
      #endif
   
      publisher->FlushBuffers();
    
      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage
            ("Propagate::TakeFinalStep complete; epoch = %16.10lf\n",
             (baseEpoch[0] + fm[0]->GetTime() / GmatTimeUtil::SECS_PER_DAY));
      #endif

   }

   // Clear previous stop conditions from the spacecraft, and then store the 
   // stop name in the spacecraft that triggered it
   for (std::vector<SpaceObject *>::iterator it = sats.begin(); 
        it != sats.end(); ++it)
      (*it)->ClearLastStopTriggered();      

   if (stopper != NULL)
   {
      // Save the stop condition and reset for next pass
      Integer stopperIndex = 0;
      for (std::vector<StopCondition*>::iterator i = stopWhen.begin(); i != stopWhen.end(); ++i)
      {
         if ((*i == stopper) || (fabs((*i)->GetStopDifference()) <= accuracy))
         {
            #ifdef DEBUG_FIRST_STEP_STOP
               MessageInterface::ShowMessage(
                  "Setting stop name '%s' on sat '%s'\n", 
                  stopper->GetName().c_str(), 
                  stopSats[stopperIndex]->GetName().c_str());
            #endif
            
            stopSats[stopperIndex]->SetLastStopTriggered((*i)->GetName());
         }
         ++stopperIndex;
      }
      triggers.clear();
   }

   for (std::vector<StopCondition *>::iterator i = stopWhen.begin(); 
        i != stopWhen.end(); ++i)
   {
      if ((*i)->GetName() == "")
      {
         StopCondition *localSc = *i;
         stopWhen.erase(i);
         delete localSc;
      }
   }

   // Toggle propagators out of final step mode
   /// @note This code should be removed if the minimum step is removed from 
   /// the RK integrators
   for (std::vector<Propagator*>::iterator current = p.begin(); 
        current != p.end(); ++current)
      (*current)->SetAsFinalStep(false);
}


//------------------------------------------------------------------------------
// Real InterpolateToStop(StopCondition *sc)
//------------------------------------------------------------------------------
/**
 * Routine that drives the cubic spline, filling the ring buffer and 
 * interpolating the time step needed to find the interval to the stop
 * condition.
 * 
 * @param <sc> The stopping condition that is used for the interpolation.
 * 
 * @return The time step to the stopping condition, as determined by the cubic
 * spline interpolator.
 */
//------------------------------------------------------------------------------
Real Propagate::InterpolateToStop(StopCondition *sc)
{
   // Now fill in the ring buffer
   Real ringStep = stopInterval / 4.0; 
   Integer ringStepsTaken = 0;
   bool firstRingStep = true;
   bool stopIsBracketed = false;
   Real elapsedSeconds = 0.0;

   while ((!stopIsBracketed) && (ringStepsTaken < 8))
   {
      // Take a fixed prop step
      if (!TakeAStep(ringStep))
         throw CommandException("Propagator Failed to Step fixed interval "
            "while filling ring buffer\n");
      elapsedSeconds += ringStep;

      // Update spacecraft for that step
      for (UnsignedInt i = 0; i < fm.size(); ++i) 
      {
         fm[i]->UpdateSpaceObject(
            baseEpoch[i] + fm[i]->GetTime() / GmatTimeUtil::SECS_PER_DAY);
      }

      // Update the data in the stop condition
      sc->SetRealParameter(stopCondEpochID, elapsedSeconds);
      stopIsBracketed = sc->AddToBuffer(firstRingStep);
      
      ++ringStepsTaken;
      firstRingStep = false;
   }

   // Now interpolate the epoch...
   stopEpoch = sc->GetStopEpoch();

   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage(
         "Propagate::TakeFinalStep set the stopEpoch = %.12lf\n", stopEpoch);
   #endif
   
   // ...and restore the spacecraft and force models
   BufferSatelliteStates(false);
   for (UnsignedInt i = 0; i < fm.size(); ++i) 
   {
      fm[i]->UpdateFromSpaceObject();
      // Back out the steps taken to build the ring buffer
      fm[i]->SetTime(fm[i]->GetTime() - ringStepsTaken * ringStep);

      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage(
            "Force model base Epoch = %.12lf  elapsedTime = %.12lf  "
            "net Epoch = %.12lf\n", baseEpoch[i], fm[i]->GetTime(), 
            baseEpoch[i] + fm[i]->GetTime() / GmatTimeUtil::SECS_PER_DAY);
      #endif
   }
   
   return stopEpoch;
}


//------------------------------------------------------------------------------
// Real RefineFinalStep(Real secsToStep, StopCondition *stopper)
//------------------------------------------------------------------------------
/**
 * Routine that refines the solution found by the cubic spline, by solving for 
 * the stopping condition using secants until the step produced falls within the
 * desired accuracy.
 * 
 * @param <secsToStep>  First guess at the duration needed for stopping.
 * @param <stopper>     The stopping condition used for the refinement.
 * 
 * @return The time step to the stopping condition, as determined by the secant
 * solver.
 */
//------------------------------------------------------------------------------
Real Propagate::RefineFinalStep(Real secsToStep, StopCondition *stopper)
{
   #ifdef DEBUG_SECANT_DETAILS
      MessageInterface::ShowMessage("\nRefineFinalStep(%16.13lf) entered.\n", 
            secsToStep);
   #endif

   bool closeEnough = false;
   bool nextTimeThrough = false;
   Integer attempts = 0;

   Real x[2], y[2], slope, target, intercept;
   Parameter *stopParam = stopper->GetStopParameter(),
             *targParam = stopper->GetGoalParameter();
   
   x[0] = x[1] = y[1] = 0.0;
   
   intercept = y[0] = stopParam->EvaluateReal();

   if (stopper->IsTimeCondition())
   {
      // Handle time based stopping condition refinement
      Real prevStep = secsToStep;
      
      while ((attempts < 50) && !closeEnough)
      {
         // Restore the spacecraft and force models to the end state of the last step
         if (attempts > 0)
         {
            BufferSatelliteStates(false);
            for (UnsignedInt i = 0; i < fm.size(); ++i) 
            {
               fm[i]->UpdateFromSpaceObject();
               fm[i]->SetTime(fm[i]->GetTime() - prevStep);
            }
         }

         if (!TakeAStep(secsToStep))
            throw CommandException("Unable to take a good step while searching "
               "for stopping step in command\n   \"" + GetGeneratingString() + 
               "\"\n");
      
         // Update spacecraft for that step
         for (UnsignedInt i = 0; i < fm.size(); ++i) 
         {
            fm[i]->UpdateSpaceObject(
               baseEpoch[i] + fm[i]->GetTime() / GmatTimeUtil::SECS_PER_DAY);
         }
   
         if (targParam != NULL)
            target = targParam->EvaluateReal();
         else
            target = stopper->GetStopGoal();
         
         x[1] = secsToStep;
         y[1] = stopParam->EvaluateReal();

         #ifdef DEBUG_STOPPING_CONDITIONS   
            MessageInterface::ShowMessage(
               "[%d] Time based secant target: %16.12le\n"
               "     BaseEpoch: %16.9lf    fmTime: %16.9lf\n"
               "     Secant data points:\n"
               "        (%16.12le, %16.12le)\n"
               "        (%16.12le, %16.12le)\n"
               "     Secant timestep: %16.12lf\n",
               attempts, target, baseEpoch[0], fm[0]->GetTime(), x[0], y[0], 
               x[1], y[1], secsToStep);
         #endif
         
         if (fabs(target - y[1]) < timeAccuracy)
            closeEnough = true;
         else
         {
            prevStep = secsToStep;
            slope = (y[1] - y[0]) / (x[1] - x[0]);
            secsToStep = (target - y[0]) / slope;
         }
         
         ++attempts;
      }
   }
   else
   {
      // Handle non-time based stopping condition refinement
      while (!closeEnough && (attempts < 50))
      {
         target = stopper->GetStopGoal();
         if (stopper->IsCyclicParameter())
         {
            y[0] = GetRangedAngle(y[0], target);
            y[1] = GetRangedAngle(y[1], target);
         }

         if (nextTimeThrough)
         {
            // Restore spacecraft and force models to end state of last step
            BufferSatelliteStates(false);
            for (UnsignedInt i = 0; i < fm.size(); ++i) 
            {
               fm[i]->UpdateFromSpaceObject();
               fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
            }
         
            if (x[1] == x[0])
            {
               #ifdef DEBUG_SECANT_DETAILS
                  MessageInterface::ShowMessage("Infinite slope error!!!\n"
                        "[%2d] Secant target: %16.12le\n"
                        "   Secant data points:\n"
                        "      (%16.12le, %16.12le)\n"
                        "      (%16.12le, %16.12le)\n"
                        "   Secant timestep: %16.12lf\n",
                        attempts, target, x[0], y[0], x[1], y[1], secsToStep);
               #endif
               Real bisectStep = 0.0;
               try
               {
                  bisectStep = BisectToStop(stopper);
               }
               catch (BaseException &ex)
               {
                  MessageInterface::ShowMessage(
                        "Error found (%s) while bisecting after a zero slope "
                        "secant was detected.\n", ex.GetFullMessage().c_str());
                  throw;
               }
               if (bisectStep == 0.0)
                  throw CommandException("Error refining timestep for Propagate"
                     " command: infinite slope in secant and bisection failed"
                     " to stop on \"" + stopper->GetName() + "\"; Exiting\n");
               secsToStep = bisectStep;
               break;
            }
            slope = (y[1] - y[0]) / (x[1] - x[0]);
            if (slope == 0.0) 
            {
               ++attempts;
               #ifdef DEBUG_SECANT_DETAILS
                  MessageInterface::ShowMessage("Zero slope error!!!\n"
                        "[%2d] Secant target: %16.12le\n"
                        "   Secant data points:\n"
                        "      (%16.12le, %16.12le)\n"
                        "      (%16.12le, %16.12le)\n"
                        "   Secant timestep: %16.12lf\n",
                        attempts, target, x[0], y[0], x[1], y[1], secsToStep);
               #endif
               
               Real bisectStep = 0.0;
               try
               {
                  bisectStep = BisectToStop(stopper);
               }
               catch (BaseException &ex)
               {
                  MessageInterface::ShowMessage(
                        "Error found (%s) while bisecting after a zero slope "
                        "secant was detected.\n", ex.GetFullMessage().c_str());
                  throw;
               }
               if (bisectStep == 0.0)
                  throw CommandException("Error refining timestep for Propagate"
                     " command: zero slope in secant and bisection failed to"
                     " stop on \"" +
                     stopper->GetName() + "\"; Exiting\n");
               secsToStep = bisectStep;
               break;
            }
            secsToStep = x[1] + (target - y[1]) / slope;

            #ifdef DEBUG_STOPPING_CONDITIONS   
               MessageInterface::ShowMessage(
                     "[%2d] Secant target: %16.12le\n"
                     "   Secant data points:\n"
                     "      (%16.12le, %16.12le)\n"
                     "      (%16.12le, %16.12le)\n"
                     "   Secant timestep: %16.12lf\n",
                     attempts, target, x[0], y[0], x[1], y[1], secsToStep);
            #endif
         }
         else
         {
            if (stopper->IsCyclicParameter())
               y[0] = GetRangedAngle(y[0], target);
         }

         #ifdef DEBUG_STOPPING_CONDITIONS
            MessageInterface::ShowMessage(
                  "Before step, param = %16.12lf, Stepping from %16.12lf by "
                  "%16.12lf, ", stopParam->EvaluateReal(), fm[0]->GetTime(), 
                  secsToStep);
         #endif
         
         if (!TakeAStep(secsToStep))
            throw CommandException(
                  "Unable to take a good step while searching "
                  "for stopping step in command\n   \"" + 
                  GetGeneratingString() + "\"\n");
         
         #ifdef DEBUG_STOPPING_CONDITIONS
            MessageInterface::ShowMessage("\nAfter step, param = %16.12lf, "
                  "Stepped to %16.12lf\n", stopParam->EvaluateReal(), 
                  fm[0]->GetTime());
         #endif         
         
            // Update spacecraft for that step
         for (UnsignedInt i = 0; i < fm.size(); ++i) 
         {
            fm[i]->UpdateSpaceObject(
               baseEpoch[i] + fm[i]->GetTime() / GmatTimeUtil::SECS_PER_DAY);
         }

         #ifdef DEBUG_SECANT_DETAILS
            MessageInterface::ShowMessage(
                  "%d %16.12lf %16.12lf %16.12lf %16.12lf", attempts, x[0], 
                  y[0], x[1], y[1]);
         #endif
            
         // Buffer data for next iteration
         if (nextTimeThrough)
         {
            x[0] = x[1];
            y[0] = y[1];
         }
         else
            nextTimeThrough = true;
         
         // And store current results
         x[1] = secsToStep;
         y[1] = stopParam->EvaluateReal();
         if (stopper->IsCyclicParameter())
            y[1] = GetRangedAngle(y[1], target);

         #ifdef DEBUG_SECANT_DETAILS
            MessageInterface::ShowMessage(" %16.12lf %16.12lf\n", x[1], y[1]);
         #endif
         
         // Check to see if accuracy is within tolerance
         if (fabs(stopper->GetStopDifference()) < stopAccuracy)
            closeEnough = true;
   
         ++attempts;
   
         #ifdef DEBUG_STOPPING_CONDITIONS   
            MessageInterface::ShowMessage(
               "   %d: secsToStep = %16.12lf, Stop diff = %16.12lf, "
               "Accuracy = %16.12lf\n", attempts, secsToStep, 
               stopper->GetStopDifference(), stopAccuracy);      
         #endif
      }
   }
   
   if (attempts == 50)
   {
      // Back out last step, then try bisection
      BufferSatelliteStates(false);
      for (UnsignedInt i = 0; i < fm.size(); ++i) 
      {
         fm[i]->UpdateFromSpaceObject();
         fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
      }

      Real bisectSecsToStep = BisectToStop(stopper);
      if (bisectSecsToStep != 0.0)
         secsToStep = bisectSecsToStep;
      else
         MessageInterface::ShowMessage(
               "WARNING: Failed to find a good stopping point for condition "
               "\"%s\" in 50 attempts, and bisection failed as well!\n", 
               stopper->GetName().c_str());
   }
   
   // Restore the spacecraft and force models to the end state of the last step
   BufferSatelliteStates(false);
   for (UnsignedInt i = 0; i < fm.size(); ++i) 
   {
      fm[i]->UpdateFromSpaceObject();
      fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
   }
      
   return secsToStep;
}

//------------------------------------------------------------------------------
// Real BisectToStop(StopCondition *stopper)
//------------------------------------------------------------------------------
/**
 * Bisection method used as a "last resort" to find stopping point.
 * 
 * @param <stopper> Stopping condition we're using.
 * 
 * @note BisectToStop method is not yet implemented
 */
//------------------------------------------------------------------------------
Real Propagate::BisectToStop(StopCondition *stopper)
{
   Integer attempts = 0, attemptsMax = 52;  // 52 bits in ANSI double
   bool closeEnough = false;
   Real secsToStep = stepBrackets[1];
   Real values[2], currentValue;
   Real target = 0.0;       // Bogus initialization to clear a warning
   Real dt = stepBrackets[1] - stepBrackets[0];
   Real increasing = 1.0;
   
   Parameter *stopParam = stopper->GetStopParameter(),
             *targParam = stopper->GetGoalParameter();
   
   currentValue = values[0] = values[1] = stopParam->EvaluateReal();

   while ((attempts < attemptsMax) && !closeEnough)
   {
      if (attempts > 0)
      {
         BufferSatelliteStates(false);
         for (UnsignedInt i = 0; i < fm.size(); ++i) 
         {
            fm[i]->UpdateFromSpaceObject();
            fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
         }
        
         dt *= 0.5;
         
         if (attempts == 1)
         {
            values[1] = currentValue;
            secsToStep = stepBrackets[0] + dt; 
            if (stopper->IsCyclicParameter())
               values[0] = GetRangedAngle(values[0], target);

            if (values[1] < values[0])
               increasing = -1.0;
         }
         else
         {
            if (currentValue > target)
            {
               secsToStep -= increasing * dt;
               if (increasing > 0.0)
                  values[1] = currentValue;
               else
                  values[0] = currentValue;
            }
            else
            {
               secsToStep += increasing * dt;
               if (increasing > 0.0)
                  values[0] = currentValue;
               else
                  values[1] = currentValue;
            }
         }
      }
      
      if (!TakeAStep(secsToStep))
         throw CommandException("Unable to take a good step while searching "
            "for stopping step in command\n   \"" + GetGeneratingString() + 
            "\"\n");
   
      // Update spacecraft for that step
      for (UnsignedInt i = 0; i < fm.size(); ++i) 
      {
         fm[i]->UpdateSpaceObject(
            baseEpoch[i] + fm[i]->GetTime() / GmatTimeUtil::SECS_PER_DAY);
      }

      if (targParam != NULL)
         target = targParam->EvaluateReal();
      else
         target = stopper->GetStopGoal();

      currentValue = stopParam->EvaluateReal();
      if (stopper->IsCyclicParameter())
         currentValue = GetRangedAngle(currentValue, target);

      #ifdef DEBUG_BISECTION_DETAILS
         if (attempts == 0)
         {
            MessageInterface::ShowMessage(
               "Bisection[%d]: Stop(%16.13lf) = %16.13lf\n\n"
               "           Stop(%16.13lf) = %16.13lf\n"
               "           Goal           = %16.13lf\n",
               attempts, stepBrackets[0], values[0], secsToStep, currentValue, 
               target);
         }
         else
         {
            MessageInterface::ShowMessage(
               "Bisection[%d]: Stop(%16.13lf) = %16.13lf\n"
               "           Stop(%16.13lf) = %16.13lf\n\n"
               "           Stop(%16.13lf) = %16.13lf\n"
               "           Goal           = %16.13lf\n",
               attempts, stepBrackets[0], values[0], stepBrackets[1], values[1], 
               secsToStep, currentValue, target);
         }
      #endif   

      ++attempts;
      
      if (fabs(target - currentValue) < stopAccuracy)
            closeEnough = true;
   }
   
   if (attempts == attemptsMax)
      secsToStep = 0.0;
   
   return secsToStep;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Resets the Propagate command to an uninitialized state.
 */
//------------------------------------------------------------------------------
void Propagate::RunComplete()
{
   if (inProgress)
      publisher->FlushBuffers();
      
   inProgress = false;
   hasFired = false;

   #ifdef DEBUG_FIRST_CALL
      firstStepFired = false;
   #endif
   
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
// void AddTransientForce(StringArray *sats, ForceModel *p)
//------------------------------------------------------------------------------
/**
 * Passes transient forces into the ForceModel(s).
 *
 * @param <sats> The array of satellites used in the ForceModel.
 * @param <p>    The current ForceModel that is receiving the forces.
 */
//------------------------------------------------------------------------------
void Propagate::AddTransientForce(StringArray *sats, ForceModel *p)
{
   // Find any transient force that is active and add it to the force model
   for (std::vector<PhysicalModel*>::iterator i = transientForces->begin();
        i != transientForces->end(); ++i) {
      StringArray tfSats = (*i)->GetRefObjectNameArray(Gmat::SPACECRAFT);
      // Loop through the spacecraft that go with the force model, ans see if 
      // they are in the spacecraft list for the current transient force
      for (StringArray::iterator current = sats->begin(); 
           current != sats->end(); ++current) {
         if (find(tfSats.begin(), tfSats.end(), *current) != tfSats.end()) {
            p->AddForce(*i);
            break;      // Avoid multiple adds
         }
      }
   }

   #ifdef DEBUG_PROPAGATE_INIT
      ForceModel *fm;
      PhysicalModel *pm;
   
      MessageInterface::ShowMessage(
         "Propagate::AddTransientForces completed; force details:\n");
      for (std::vector<PropSetup*>::iterator p = prop.begin(); 
           p != prop.end(); ++p) {
         fm = (*p)->GetForceModel();
         if (!fm)
            throw CommandException("ForceModel not set in PropSetup \"" + 
                                   (*p)->GetName() + "\"");
         MessageInterface::ShowMessage(
            "   Forces in %s:\n", fm->GetName().c_str());
         for (Integer i = 0; i < fm->GetNumForces(); ++i) {
            pm = fm->GetForce(i);
            MessageInterface::ShowMessage(
               "      %s   %s\n", pm->GetTypeName().c_str(),
               pm->GetName().c_str());
         }
      }
   #endif
}


//------------------------------------------------------------------------------
// void ClearTransientForce()
//------------------------------------------------------------------------------
/**
 * Removes transient forces from the ForceModel(s) after propagation.
 */
//------------------------------------------------------------------------------
void Propagate::ClearTransientForces()
{
   ForceModel *fm;
   PhysicalModel *pm;
   
   // Loop through the forces in each force model, and remove transient ones
   for (std::vector<PropSetup*>::iterator p = prop.begin(); 
        p != prop.end(); ++p) {
      fm = (*p)->GetForceModel();
      if (!fm)
         throw CommandException("ForceModel not set in PropSetup \"" + 
                                (*p)->GetName() + "\"");
      for (Integer i = 0; i < fm->GetNumForces(); ++i) {
         pm = fm->GetForce(i);
         if (pm->IsTransient()) {
            fm->DeleteForce(pm->GetName());
            i--;  // Since fm->DeleteForce() resets the size (loj: 2/15/07)
         }
      }
   }
   
   #ifdef DEBUG_PROPAGATE_INIT
      MessageInterface::ShowMessage(
         "Propagate::ClearTransientForces completed; force details:\n");
      for (std::vector<PropSetup*>::iterator p = prop.begin(); 
           p != prop.end(); ++p) {
         fm = (*p)->GetForceModel();
         if (!fm)
            throw CommandException("ForceModel not set in PropSetup \"" + 
                                   (*p)->GetName() + "\"");
         MessageInterface::ShowMessage(
            "   Forces in %s:\n", fm->GetName().c_str());
         for (Integer i = 0; i < fm->GetNumForces(); ++i) {
            pm = fm->GetForce(i);
            MessageInterface::ShowMessage(
               "      %s   %s\n", pm->GetTypeName().c_str(),
               pm->GetName().c_str());
         }
      }
   #endif
}


//------------------------------------------------------------------------------
// void SetNames(const std::string& name, StringArray& owners,
//               StringArray& elements)
//------------------------------------------------------------------------------
/**
 * Sets the parameter names used when publishing Spacecraft data.
 *
 * @param <name>     Name of the Spacecraft that is referenced.
 * @param <owners>   Array of published data identifiers.
 * @param <elements> Individual elements of the published data.
 */
//------------------------------------------------------------------------------
void Propagate::SetNames(const std::string& name, StringArray& owners,
                         StringArray& elements)
{
   // Add satellite labels
   for (Integer i = 0; i < 6; ++i)
      owners.push_back(name);       // X, Y, Z, Vx, Vy, Vz
      
   elements.push_back(name+".X");
   elements.push_back(name+".Y");
   elements.push_back(name+".Z");
   elements.push_back(name+".Vx");
   elements.push_back(name+".Vy");
   elements.push_back(name+".Vz");
}


//------------------------------------------------------------------------------
// void AddToBuffer(SpaceObject *so)
//------------------------------------------------------------------------------
/**
 * Adds satellites and formations to the state buffer.
 * 
 * @param <so> The SpaceObject that is added.
 */
//------------------------------------------------------------------------------
void Propagate::AddToBuffer(SpaceObject *so)
{
   #ifdef DEBUG_STOPPING_CONDITIONS
      MessageInterface::ShowMessage("Buffering states for '%s'\n", 
         so->GetName().c_str());
   #endif
   
   if (so->IsOfType(Gmat::SPACECRAFT))
   {
      satBuffer.push_back((Spacecraft *)(so->Clone()));
   }
   else if (so->IsOfType(Gmat::FORMATION))
   {
      Formation *form = (Formation*)so;
      formBuffer.push_back((Formation *)(so->Clone()));
      StringArray formSats = form->GetStringArrayParameter("Add");
      
      for (StringArray::iterator i = formSats.begin(); i != formSats.end(); ++i)
         AddToBuffer((SpaceObject *)(FindObject(*i)));
   }
   else
      throw CommandException("Object " + so->GetName() + " is not either a "
         "Spacecraft or a Formation; cannot buffer the object for propagator "
         "stopping conditions.");
}


//------------------------------------------------------------------------------
// void EmptyBuffer()
//------------------------------------------------------------------------------
/**
 * Cleans up the satellite state buffer.
 */
//------------------------------------------------------------------------------
void Propagate::EmptyBuffer()
{
   for (std::vector<Spacecraft *>::iterator i = satBuffer.begin(); 
        i != satBuffer.end(); ++i)
   {
      delete (*i);
   }
   satBuffer.clear();
   
   for (std::vector<Formation *>::iterator i = formBuffer.begin(); 
        i != formBuffer.end(); ++i)
   {
      delete (*i);
   }
   formBuffer.clear();
}

//------------------------------------------------------------------------------
// void BufferSatelliteStates(bool fillingBuffer)
//------------------------------------------------------------------------------
/**
 * Preserves satellite state data so it can be restored after interpolating the 
 * stopping condition propagation time.
 * 
 * @param <fillingBuffer> Flag used to indicate the fill direction.
 */
//------------------------------------------------------------------------------
void Propagate::BufferSatelliteStates(bool fillingBuffer)
{
   Spacecraft *fromSat, *toSat;
   Formation *fromForm, *toForm;
   std::string soName;
   
   for (std::vector<Spacecraft *>::iterator i = satBuffer.begin(); 
        i != satBuffer.end(); ++i)
   {
      soName = (*i)->GetName();
      if (fillingBuffer)
      {
         fromSat = (Spacecraft *)FindObject(soName);
         toSat = *i;
      }
      else
      {
         fromSat = *i;
         toSat = (Spacecraft *)FindObject(soName);
      }

      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage(
            "   Sat is %s, fill direction is %s; fromSat epoch = %.12lf   "
            "toSat epoch = %.12lf\n",
            fromSat->GetName().c_str(),
            (fillingBuffer ? "from propagator" : "from buffer"),
            fromSat->GetRealParameter("A1Epoch"), 
            toSat->GetRealParameter("A1Epoch"));

         MessageInterface::ShowMessage(
            "   '%s' Satellite state:\n", fromSat->GetName().c_str());
         Real *satrv = fromSat->GetState().GetState();
         MessageInterface::ShowMessage(
            "      %.12lf  %.12lf  %.12lf\n      %.12lf  %.12lf  %.12lf\n",
            satrv[0], satrv[1], satrv[2], satrv[3], satrv[4], satrv[5]);
      #endif
      
      (*toSat) = (*fromSat);
      
      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage(
            "After copy, From epoch %.12lf to epoch %.12lf\n",
            fromSat->GetRealParameter("A1Epoch"), 
            toSat->GetRealParameter("A1Epoch"));
      #endif      
   }

   for (std::vector<Formation *>::iterator i = formBuffer.begin(); 
        i != formBuffer.end(); ++i)
   {
      soName = (*i)->GetName();
      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage("Buffering formation %s, filling = %s\n", 
            soName.c_str(), (fillingBuffer?"true":"false"));
      #endif
      if (fillingBuffer)
      {
         fromForm = (Formation *)FindObject(soName);
         toForm = *i;
      }
      else
      {
         fromForm = *i;
         toForm = (Formation *)FindObject(soName);
      }

      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage(
            "   Formation is %s, fill direction is %s; fromForm epoch = %.12lf"
            "   toForm epoch = %.12lf\n",
            fromForm->GetName().c_str(),
            (fillingBuffer ? "from propagator" : "from buffer"),
            fromForm->GetRealParameter("A1Epoch"), 
            toForm->GetRealParameter("A1Epoch"));
      #endif
      
      (*toForm) = (*fromForm);
      
      toForm->UpdateState();
      
      #ifdef DEBUG_STOPPING_CONDITIONS
         Integer count = fromForm->GetStringArrayParameter("Add").size();

         MessageInterface::ShowMessage(
            "After copy, From epoch %.12lf to epoch %.12lf\n",
            fromForm->GetRealParameter("A1Epoch"), 
            toForm->GetRealParameter("A1Epoch"));

         MessageInterface::ShowMessage(
            "   %s for '%s' Formation state:\n", 
            (fillingBuffer ? "Filling buffer" : "Restoring states"),
            fromForm->GetName().c_str());

         Real *satrv = fromForm->GetState().GetState();
         
         for (Integer i = 0; i < count; ++i)
            MessageInterface::ShowMessage(
               "      %d:  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf\n",
               i, satrv[i*6], satrv[i*6+1], satrv[i*6+2], satrv[i*6+3], 
               satrv[i*6+4], satrv[i*6+5]);
      #endif      
   }
   
   #ifdef DEBUG_STOPPING_CONDITIONS
      for (std::vector<Spacecraft *>::iterator i = satBuffer.begin(); 
           i != satBuffer.end(); ++i)
         MessageInterface::ShowMessage(
            "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(), 
            (*i)->GetRealParameter("A1Epoch"));
   #endif
}



//------------------------------------------------------------------------------
// Real GetRangedAngle(const Real angle, const Real midpt)
//------------------------------------------------------------------------------
/**
 * Puts a cyclic parameter into its valid range.  Currently only implemented for 
 * angles.
 * 
 * @param <angle> The parameter value.
 * @param <midpt> The center of the range.
 * @param <min>   The minimum of the nominal range.  Not yet implemented.
 * @param <max>   The maximum of the nominal range.  Not yet implemented.
 * 
 * @return The ranged value.
 */
//------------------------------------------------------------------------------
Real Propagate::GetRangedAngle(const Real angle, const Real midpt)
{
   #ifdef DEBUG_STOPPING_CONDITIONS
      MessageInterface::ShowMessage(
         "Setting angle range for %.12lf around %.12lf\n", angle, midpt);
   #endif
   
   return AngleUtil::PutAngleInDegRange(angle, midpt - GmatMathUtil::PI_DEG, 
                                    midpt + GmatMathUtil::PI_DEG);
}
