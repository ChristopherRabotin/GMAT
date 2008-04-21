//$Id$
//------------------------------------------------------------------------------
//                                  Interpreter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/08/28
// Rework:  2006/09/27 by Linda Jun (NASA/GSFC)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class implementation for the Interpreter base class
 */
//------------------------------------------------------------------------------

#include "Interpreter.hpp"    // class's header file
#include "Moderator.hpp"
#include "StringTokenizer.hpp"
#include "ConditionalBranch.hpp"
#include "StringUtil.hpp"     // for ToReal()
#include "Array.hpp"
#include "Assignment.hpp"     // for GetLHS(), GetRHS()
#include "Validator.hpp"

#include "MessageInterface.hpp"
#include "NumberWrapper.hpp"
#include "ParameterWrapper.hpp"
#include "VariableWrapper.hpp"
#include "ObjectPropertyWrapper.hpp"
#include "ArrayWrapper.hpp"
#include "ArrayElementWrapper.hpp"
#include "StringObjectWrapper.hpp"
#include "BooleanWrapper.hpp"
#include "StringWrapper.hpp"
#include "OnOffWrapper.hpp"
#include "ObjectWrapper.hpp"
#include "FileUtil.hpp"       // for DoesFileExist()
#include <stack>              // for checking matching begin/end control logic
#include <fstream>            // for checking GmatFunction declaration
#include <sstream>            // for checking GmatFunction declaration

//#define __DO_NOT_USE_OBJ_TYPE_NAME__

//#define DEBUG_INIT
//#define DEBUG_COMMAND_LIST
//#define DEBUG_OBJECT_LIST
//#define DEBUG_ARRAY_GET
//#define DEBUG_CREATE_OBJECT
//#define DEBUG_CREATE_PARAM
//#define DEBUG_CREATE_ARRAY
//#define DEBUG_CREATE_COMMAND
//#define DEBUG_ASSIGNMENT
//#define DEBUG_ASSEMBLE_COMMAND
//#define DEBUG_ASSEMBLE_CREATE
//#define DEBUG_ASSEMBLE_FOR
//#define DEBUG_ASSEMBLE_CALL_FUNCTION
//#define DEBUG_ASSEMBLE_REPORT_COMMAND
//#define DEBUG_SET
//#define DEBUG_SET_FORCE_MODEL
//#define DEBUG_SET_SOLAR_SYS
//#define DEBUG_FINAL_PASS
//#define DEBUG_FINAL_PASS_CS
//#define DEBUG_FIND_OBJECT
//#define DEBUG_CHECK_OBJECT
//#define DEBUG_CHECK_BRANCH
//#define DEBUG_SPECIAL_CASE
//#define DEBUG_VALIDATE_COMMAND
//#define DEBUG_WRAPPERS
//#define DEBUG_HANDLE_ERROR
//#define DEBUG_PARSE_REPORT
//#define DBGLVL_FUNCTION_DEF 2

//------------------------------------------------------------------------------
// Interpreter(SolarSystem *ss = NULL, StringObjectMap *objMap = NULL)
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param  ss  The solar system to be used for findnig bodies
 * @param  objMap  The object map to be used for finding object 
 */
//------------------------------------------------------------------------------
Interpreter::Interpreter(SolarSystem *ss, StringObjectMap *objMap)
{
   initialized = false;
   continueOnError = true;   
   parsingDelayedBlock = false;
   ignoreError = false;
   inFunctionMode = false;
   hasFunctionDefinition = false;
   currentFunction = NULL;
   theSolarSystem = NULL;
   theObjectMap = NULL;
   
   theModerator  = Moderator::Instance();
   theReadWriter = ScriptReadWriter::Instance();
   
   if (ss)
   {
      theSolarSystem = ss;
      theValidator.SetSolarSystem(ss);
   }
   
   if (objMap)
   {
      theObjectMap = objMap;
      theValidator.SetObjectMap(objMap);
   }
   
   #ifdef DEBUG_INTERP
   MessageInterface::ShowMessage
      ("Interpreter::Interpreter() initialized=%d, theModerator=%p\n",
       initialized, theModerator);
   #endif
   
}


//------------------------------------------------------------------------------
// Interpreter()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Interpreter::~Interpreter()
{
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Builds core lists of available objects.
 */
//------------------------------------------------------------------------------
void Interpreter::Initialize()
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage
      ("Interpreter::Initialize() initialized=%d\n", initialized);
   #endif
   
   errorList.clear();
   delayedBlocks.clear();
   delayedBlockLineNumbers.clear();
   inCommandMode = false;
   parsingDelayedBlock = false;
   ignoreError = false;
   
   if (initialized)
      return;
   
   // Build a mapping for all of the defined commands
   StringArray cmds = theModerator->GetListOfFactoryItems(Gmat::COMMAND);
   copy(cmds.begin(), cmds.end(), back_inserter(commandList));
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("Number of commands = %d\n", cmds.size());
   #endif
   
   #ifdef DEBUG_COMMAND_LIST
      std::vector<std::string>::iterator pos1;
      
      MessageInterface::ShowMessage("\nCommands:\n   ");      
      for (pos1 = cmds.begin(); pos1 != cmds.end(); ++pos1)
         MessageInterface::ShowMessage(*pos1 + "\n   ");
      
   #endif
      
   if (cmds.size() == 0)
   {
      throw InterpreterException("Command list is empty.");
   }
   
   // Build a mapping for all of the defined objects
   StringArray atms = theModerator->GetListOfFactoryItems(Gmat::ATMOSPHERE);
   copy(atms.begin(), atms.end(), back_inserter(atmosphereList));
   
   StringArray atts = theModerator->GetListOfFactoryItems(Gmat::ATTITUDE);
   copy(atts.begin(), atts.end(), back_inserter(attitudeList));
   
   StringArray axes = theModerator->GetListOfFactoryItems(Gmat::AXIS_SYSTEM);
   copy(axes.begin(), axes.end(), back_inserter(axisSystemList));
   
   StringArray burns = theModerator->GetListOfFactoryItems(Gmat::BURN);
   copy(burns.begin(), burns.end(), back_inserter(burnList));
   
   StringArray cals = theModerator->GetListOfFactoryItems(Gmat::CALCULATED_POINT);
   copy(cals.begin(), cals.end(), back_inserter(calculatedPointList));
   
   StringArray fns = theModerator->GetListOfFactoryItems(Gmat::FUNCTION);
   copy(fns.begin(), fns.end(), back_inserter(functionList));
   
   StringArray hws = theModerator->GetListOfFactoryItems(Gmat::HARDWARE);
   copy(hws.begin(), hws.end(), back_inserter(hardwareList));
   
   StringArray parms = theModerator->GetListOfFactoryItems(Gmat::PARAMETER);
   copy(parms.begin(), parms.end(), back_inserter(parameterList));
   
   StringArray props = theModerator->GetListOfFactoryItems(Gmat::PROPAGATOR);
   copy(props.begin(), props.end(), back_inserter(propagatorList));
   
   StringArray forces = theModerator->GetListOfFactoryItems(Gmat::PHYSICAL_MODEL);
   copy(forces.begin(), forces.end(), back_inserter(physicalModelList));
   
   StringArray solvers = theModerator->GetListOfFactoryItems(Gmat::SOLVER);
   copy(solvers.begin(), solvers.end(), back_inserter(solverList));
   
   StringArray stops = theModerator->GetListOfFactoryItems(Gmat::STOP_CONDITION);
   copy(stops.begin(), stops.end(), back_inserter(stopcondList));
   
   StringArray subs = theModerator->GetListOfFactoryItems(Gmat::SUBSCRIBER);
   copy(subs.begin(), subs.end(), back_inserter(subscriberList));
   
   
   #ifdef DEBUG_OBJECT_LIST
      std::vector<std::string>::iterator pos;
      
      MessageInterface::ShowMessage("\nAtmosphereModel:\n   ");
      for (pos = atms.begin(); pos != atms.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");

      MessageInterface::ShowMessage("\nAttitudes:\n   ");
      for (pos = atts.begin(); pos != atts.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");

      MessageInterface::ShowMessage("\nAxisSystems:\n   ");
      for (pos = axes.begin(); pos != axes.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nBurns:\n   ");
      for (pos = burns.begin(); pos != burns.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nCalculatedPoints:\n   ");
      for (pos = cals.begin(); pos != cals.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nFunctions:\n   ");
      for (pos = fns.begin(); pos != fns.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nHardwares:\n   ");
      for (pos = hws.begin(); pos != hws.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nPhysicalModels:\n   ");
      for (pos = forces.begin(); pos != forces.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nParameters:\n   ");
      for (pos = parms.begin();  pos != parms.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nPropagators:\n   ");
      for (std::vector<std::string>::iterator pos = props.begin();
           pos != props.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");

      MessageInterface::ShowMessage("\nSolvers:\n   ");
      for (pos = solvers.begin(); pos != solvers.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");

      MessageInterface::ShowMessage("\nStopConds:\n   ");
      for (pos = stops.begin(); pos != stops.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");

      MessageInterface::ShowMessage("\nSubscribers:\n   ");
      for (pos = subs.begin(); pos != subs.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");

      MessageInterface::ShowMessage("\n");
   #endif
   
   // Retister alias used in scripting
   RegisterAliases();
   
   // Initialize TextParser command list
   theTextParser.Initialize(commandList);
   
   initialized = true;
}


//------------------------------------------------------------------------------
// void RegisterAliases()
//------------------------------------------------------------------------------
/*
 * Some GMAT script identifiers can be accessed using multiple text string.
 * This method creates a mapping for these strings so that scripts can be parsed
 * correctly.
 */
//------------------------------------------------------------------------------
void Interpreter::RegisterAliases()
{
   ForceModel::SetScriptAlias("PrimaryBodies", "GravityField");
   ForceModel::SetScriptAlias("Gravity", "GravityField");
   ForceModel::SetScriptAlias("PointMasses", "PointMassForce");
   ForceModel::SetScriptAlias("Drag", "DragForce");
   ForceModel::SetScriptAlias("SRP", "SolarRadiationPressure");
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfObjects(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type.
 *
 * @param  type  object type
 *
 * @return array of configured item names; return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& Interpreter::GetListOfObjects(Gmat::ObjectType type)
{
   return theModerator->GetListOfObjects(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetConfiguredObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Interpreter::GetConfiguredObject(const std::string &name)
{
   return theModerator->GetConfiguredObject(name);
}


//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &type, const std::string &name,
//                        bool manage)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to build core objects and put them in the ConfigManager.
 *  
 * @param  type  Type for the requested object.
 * @param  name  Name for the object
 * @param  manage  true if created object to be added to configuration (true)
 *
 * @return object pointer on success, NULL on failure.
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::CreateObject(const std::string &type,
                                    const std::string &name,
                                    bool manage)
{
   #ifdef DEBUG_CREATE_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::CreateObject() type=<%s>, name=<%s>, manage=%d\n",
       type.c_str(), name.c_str(), manage);
   #endif
   
   GmatBase *obj = NULL;
   
   // if object to be managed and has non-blank name, and name is not valid, handle error
   if (manage && name != "")
   {
      bool isValid = false;
      
      // if type is Array, set flag to ignore bracket
      if (type == "Array")
         isValid = GmatStringUtil::IsValidName(name, true);
      else
         isValid = GmatStringUtil::IsValidName(name, false);
      
      if (!isValid)
      {
         InterpreterException ex
            (type + " object can not be named to \"" + name + "\"");
         HandleError(ex);
         return NULL;
      }
   }
   
   // Go through more checking if name is not blank
   if (name != "")
   {
      // object name cannot be any of command names
      if (find(commandList.begin(), commandList.end(), name) != commandList.end())
      {
         InterpreterException ex
            (type + " object can not be named to Command \"" + name + "\"");
         HandleError(ex);
         return NULL;
      }
      
      #ifdef __DO_NOT_USE_OBJ_TYPE_NAME__
      // object name cannot be any of object types
      if (IsObjectType(name))
      {
         InterpreterException ex
            (type + " object can not be named to Object Type \"" + name + "\"");
         HandleError(ex);
         return NULL;
      }
      #endif
      
      // Give warning if name already exist
      if ((name != "EarthMJ2000Eq") && 
          (name != "EarthMJ2000Ec") && 
          (name != "EarthFixed"))
      {
         obj = FindObject(name);
         // Since System Parameters are created automatically as they are referenced,
         // do not give warning if creating a system parameter
         if (obj != NULL && ((obj->GetType() != Gmat::PARAMETER) ||
                             (obj->GetType() == Gmat::PARAMETER &&
                              (!obj->IsOfType("SystemParameter")))))
         {
            InterpreterException ex("");
            ex.SetDetails("%s object named \"%s\" already exist",
                          type.c_str(), name.c_str());
            HandleError(ex, true, true);
            return obj;
         }
      }
   }
   
   
   if (type == "Spacecraft") 
      obj = (GmatBase*)theModerator->CreateSpacecraft(type, name);
   
   else if (type == "Formation") 
      obj = (GmatBase*)theModerator->CreateSpacecraft(type, name);
   
   else if (type == "PropSetup") 
      obj = (GmatBase*)theModerator->CreatePropSetup(name);
   
   else if (type == "ForceModel") 
      obj = (GmatBase*)theModerator->CreateForceModel(name);
   
   else if (type == "CoordinateSystem") 
      obj = (GmatBase*)theModerator->CreateCoordinateSystem(name, true);
   
   else
   {
      // Handle Propagator
      if (find(propagatorList.begin(), propagatorList.end(), type) != 
          propagatorList.end())
         obj = (GmatBase*)theModerator->CreatePropagator(type, name);
      
      // Handle AxisSystem
      else if (find(axisSystemList.begin(), axisSystemList.end(), type) != 
               axisSystemList.end())
         obj =(GmatBase*) theModerator->CreateAxisSystem(type, name);
      
      // Handle Atmosphere Model
      else if (find(atmosphereList.begin(), atmosphereList.end(), type) != 
               atmosphereList.end())
         obj = (GmatBase*)theModerator->CreateAtmosphereModel(type, name);
      
      // Handle Attitude
      else if (find(attitudeList.begin(), attitudeList.end(), type) != 
               attitudeList.end())
         obj = (GmatBase*)theModerator->CreateAttitude(type, name);
      
      // Handle Burns
      else if (find(burnList.begin(), burnList.end(), type) != 
               burnList.end())
         obj = (GmatBase*)theModerator->CreateBurn(type, name);
      
      // Handle CalculatedPoint (Barycenter, LibrationPoint)
      // Creates default Barycentor or LibrationPoint
      else if (find(calculatedPointList.begin(), calculatedPointList.end(), type) != 
               calculatedPointList.end())
         obj =(GmatBase*) theModerator->CreateCalculatedPoint(type, name, true);
      
      // Handle Functions
      else if (find(functionList.begin(), functionList.end(), type) != 
               functionList.end())
         obj = (GmatBase*)theModerator->CreateFunction(type, name);
      
      // Handle Hardware (tanks, thrusters, etc.)
      else if (find(hardwareList.begin(), hardwareList.end(), type) != 
               hardwareList.end())
         obj = (GmatBase*)theModerator->CreateHardware(type, name);
      
      // Handle Parameters
      else if (find(parameterList.begin(), parameterList.end(), type) != 
               parameterList.end())
         obj = (GmatBase*)CreateParameter(type, name, "", "", manage);
      
      // Handle PhysicalModel
      else if (find(physicalModelList.begin(), physicalModelList.end(), type) != 
               physicalModelList.end())
         obj = (GmatBase*)theModerator->CreatePhysicalModel(type, name);
      
      // Handle Solvers
      else if (find(solverList.begin(), solverList.end(), type) != 
               solverList.end())
         obj = (GmatBase*)theModerator->CreateSolver(type, name);
      
      // Handle Subscribers
      else if (find(subscriberList.begin(), subscriberList.end(), type) != 
               subscriberList.end())
         obj = (GmatBase*)theModerator->CreateSubscriber(type, name);
   
   }
   
   //@note
   // Do throw exception if obj == NULL, since caller uses return pointer
   // to test further.
   
   
   #ifdef DEBUG_CREATE_OBJECT
   if (obj != NULL)
   {
      MessageInterface::ShowMessage
         ("Interpreter::CreateObject() type=<%s>, name=<%s> successfully created\n",
          obj->GetTypeName().c_str(), obj->GetName().c_str());
   }
   #endif
   
   return obj;
}


//------------------------------------------------------------------------------
// void SetSolarSystemInUse(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets a current solar system in use.
 *
 * @param <ss> Pointer to the solar system
 *
 */
//------------------------------------------------------------------------------
void Interpreter::SetSolarSystemInUse(SolarSystem *ss)
{
   #ifdef DEBUG_SET_SOLAR_SYS
   MessageInterface::ShowMessage
      ("Interpreter::SetSolarSystemInUse() ss=<%p>\n", ss);
   #endif
   
   if (ss != NULL)
   {
      theSolarSystem = ss;
      theValidator.SetSolarSystem(ss);
   }
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse()
//------------------------------------------------------------------------------
/**
 * Retrieves a current solar system in use.
 *
 * @return a default solar system object pointer
 */
//------------------------------------------------------------------------------
SolarSystem* Interpreter::GetSolarSystemInUse()
{
   return theSolarSystem;
}


//------------------------------------------------------------------------------
// void SetObjectMap(StringObjectMap *objMap)
//------------------------------------------------------------------------------
/**
 * Sets object map to be used for finding objects.
 * 
 * @param <objMap> Pointer to the object map
 */
//------------------------------------------------------------------------------
void Interpreter::SetObjectMap(StringObjectMap *objMap)
{
   #ifdef DEBUG_SET_OBJECT_MAP
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectMap() objMap=<%p>\n", objMap);
   #endif
   
   if (objMap != NULL)
   {
      theObjectMap = objMap;
      theValidator.SetObjectMap(objMap);
   }
}


//------------------------------------------------------------------------------
// StringObjectMap* GetObjectMap()
//------------------------------------------------------------------------------
/**
 * @return a current object map in use.
 */
//------------------------------------------------------------------------------
StringObjectMap* Interpreter::GetObjectMap()
{
   return theObjectMap;
}


//------------------------------------------------------------------------------
// bool CheckUndefinedReference(GmatBase *obj, bool writeLine)
//------------------------------------------------------------------------------
/*
 * This method checks if reference object of given object exist through
 * the Validator.
 *
 * @param  obj  input object of undefined reference object to be checked
 * @param  writeLine  flag indicating whether or not line number should be written
 *                    for ther error message
 */
//------------------------------------------------------------------------------
bool Interpreter::CheckUndefinedReference(GmatBase *obj, bool writeLine)
{
   bool isValid = theValidator.CheckUndefinedReference(obj, continueOnError);
   
   // Handle error messages here
   if (!isValid)
   {
      StringArray errList = theValidator.GetErrorList();
      for (UnsignedInt i=0; i<errList.size(); i++)
         HandleError(InterpreterException(errList[i]), writeLine);
   }
   
   return isValid;
}


//------------------------------------------------------------------------------
// bool ValidateCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Checks the input command to make sure it wrappers are set up for it
 * correctly through the Validator, if necessary.
 *
 * @param  cmd  the command to validate
 */
//------------------------------------------------------------------------------
bool Interpreter::ValidateCommand(GmatCommand *cmd)
{
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::ValidateCommand() cmd=<%p><%s>\n", cmd,
       cmd->GetTypeName().c_str());
   #endif
   
   // if in function mode, do not validate command (loj: 2008.03.12)
   if (inFunctionMode)
   {
      #ifdef DEBUG_VALIDATE_COMMAND
      MessageInterface::ShowMessage
         ("   Interpreter is in function mode, so just returning true.\n");
      #endif
      
      return true;
   }
   
   bool isValid = theValidator.ValidateCommand(cmd, continueOnError);
   
   // Handle error messages here
   if (!isValid)
   {
      StringArray errList = theValidator.GetErrorList();
      for (UnsignedInt i=0; i<errList.size(); i++)
         HandleError(InterpreterException(errList[i]));
   }
   
   return isValid;
   
} // ValidateCommand()


//------------------------------------------------------------------------------
// bool ValidateSubscriber(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Checks the input subscriber to make sure it wrappers are set up for it
 * correctly, if necessary.
 *
 * @param <obj> the subscriber to validate
 */
//------------------------------------------------------------------------------
bool Interpreter::ValidateSubscriber(GmatBase *obj)
{
   if (obj == NULL)
      throw InterpreterException("The subscriber object to be validated is NULL");
   
   // Now continue validation
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Interpreter::ValidateSubscriber() obj=<%p><%s>\n", obj,
       obj->GetName().c_str());
   #endif
   
   // This method can be called from other than Interpreter, so check if
   // object is SUBSCRIBER type
   if (obj->GetType() != Gmat::SUBSCRIBER)
   {
      InterpreterException ex
         ("ElementWrapper for \"" + obj->GetName() + "\" of type \"" +
          obj->GetTypeName() + "\" cannot be created.");
      HandleError(ex);
      return false;
   }
   
   Subscriber *sub = (Subscriber*)obj;
   sub->ClearWrappers();
   const StringArray wrapperNames = sub->GetWrapperObjectNameArray();
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage("In ValidateSubscriber, wrapper names are:\n");
   for (Integer ii=0; ii < (Integer) wrapperNames.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", wrapperNames[ii].c_str());
   #endif
   
   for (StringArray::const_iterator i = wrapperNames.begin();
        i != wrapperNames.end(); ++i)
   {
      try
      {
         ElementWrapper *ew = theValidator.CreateElementWrapper(*i, true);
         
         if (sub->SetElementWrapper(ew, *i) == false)
         {
            InterpreterException ex
               ("ElementWrapper for \"" + (*i) +
                "\" cannot be created for the Subscriber \"" + obj->GetName() + "\"");
            HandleError(ex, false);
            return false;
         }
      }
      catch (BaseException &ex)
      {
         HandleError(ex);
         return false;
      }
   }
   
   return true;
   
} // ValidateSubscriber()


//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// bool FindPropertyID(GmatBase *obj, const std::string &chunk, GmatBase **owner,
//                     Integer &id, Gmat::ParameterType &type)
//------------------------------------------------------------------------------
/*
 * Finds property ID for given property. If property not found in the obj,
 * it tries to find proerty from the owned objects.
 *
 * @param  obj    Object to find proerty
 * @param  chunk  String contains property
 * @param  owner  Address of new owner pointer to be returned
 * @param  id     Property ID to return (-1 if property not found)
 * @param  type   Property type to return
 *                (Gmat::UNKNOWN_ParameterType if property not found)
 *
 * @return true if property found
 *
 * For example, From "FM.Gravity.Earth.Model"
 *   obj is FM pointer, chunk is "Gravity.Earth.Model"
 */
//------------------------------------------------------------------------------
bool Interpreter::FindPropertyID(GmatBase *obj, const std::string &chunk,
                                 GmatBase **owner, Integer &id,
                                 Gmat::ParameterType &type)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::FindPropertyID() obj=<%p><%s>, chunk=<%s>\n", obj,
       (obj == NULL ? "NULL" : obj->GetName().c_str()), chunk.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(chunk);
   Integer count = parts.size();
   std::string prop = parts[count-1];
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("   property=%s\n", prop.c_str());
   #endif
   
   // Set initial output id and type
   id = -1;
   type = Gmat::UNKNOWN_PARAMETER_TYPE;
   
   try
   {
      id = obj->GetParameterID(prop);
      type = obj->GetParameterType(id);
      *owner = obj;
      retval = true;
   }
   catch (BaseException &e)
   {
      if (FindOwnedObject(obj, prop, owner, id, type))
         retval = true;
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::FindPropertyID() returning owner=<%p><%s>, retval=%d\n",
       *owner, ((*owner) == NULL ? "NULL" : (*owner)->GetName().c_str()), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name, const std::string &ofType = "")
//------------------------------------------------------------------------------
/**
 * Finds the object from the current object map.
 * (old method: Calls the Moderator to find a configured object.)
 *
 * @param  name    Name of the object.
 * @param  ofType  Type of object required; leave blank for no checking
 *
 * @return  object pointer found
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::FindObject(const std::string &name, 
                                  const std::string &ofType)
{
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::FindObject() entered: name='%s'\n", name.c_str());
   #endif
   
   if (theObjectMap == NULL)
      throw InterpreterException("The Object Map is not set in the Interpreter.\n");
   
   if (theSolarSystem == NULL)
      throw InterpreterException("The Solar System is not set in the Interpreter.\n");
   
   if (name == "")
      return NULL;
   
   if (name == "SolarSystem")
      return theSolarSystem;
   
   GmatBase *obj = NULL;
   std::string newName = name;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find_first_of("([");
   if (index != name.npos)
   {
      newName = name.substr(0, index);
      
      #ifdef DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage
         ("Interpreter::FindObject() entered: newName=%s\n", newName.c_str());
      #endif
   }
   
   // Find object from the object map
   if (theObjectMap->find(newName) != theObjectMap->end())
      if ((*theObjectMap)[newName]->GetName() == newName)
         obj = (*theObjectMap)[newName];
   
   // try SolarSystem if obj is still NULL
   if (obj == NULL)
      obj = (GmatBase*)(theSolarSystem->GetBody(newName));
   
   // check for the requested type
   if ((obj != NULL) && (ofType != "") && (!obj->IsOfType(ofType)))
      obj = NULL;
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::FindObject() returning <%p><%s><%s>\n", obj,
       (obj == NULL) ? "NULL" : obj->GetTypeName().c_str(),
       (obj == NULL) ? "NULL" : obj->GetName().c_str());
   #endif
   
   return obj;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &type, const std::string &desc)
//------------------------------------------------------------------------------
GmatCommand* Interpreter::CreateCommand(const std::string &type,
                                        const std::string &desc, bool &retFlag,
                                        GmatCommand *inCmd)
{
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::CreateCommand() type=<%s>, inCmd=%p, \n   desc=<%s>\n",
       type.c_str(), inCmd, desc.c_str());
   MessageInterface::ShowMessage
      ("   inFunctionMode=%d, hasFunctionDefinition=%d\n", inFunctionMode,
       hasFunctionDefinition);
   #endif
   
   GmatCommand *cmd = NULL;
   std::string desc1 = desc;
   bool commandFound = false;
   
   if (find(commandList.begin(), commandList.end(), type)
       != commandList.end())
      commandFound = true;
   
   
   // Check for CallFunction
   if (type[0] == '[')
   {
      cmd = AppendCommand("CallFunction", retFlag, inCmd);
      desc1 = type +  "=" + desc;
      cmd->SetGeneratingString(desc1);
   }
   /// @TODO: This is a work around for a call function
   /// without any return parameters.  It should be updated in
   /// the design to handle this situation.
   else if ((desc1.find("=") == desc1.npos) && (desc != "")
            && (!commandFound))
   {
      StringArray parts = theTextParser.SeparateSpaces(desc1);
      //WriteParts("Calling IsObjectType()", parts);
      
      // Check for valid object type first before creating a command,
      // since UnknownCommand Variable var1 still creates CallFunction command
      
      if (IsObjectType(parts[0]))
      {
         InterpreterException ex
            ("Found invalid command \"" + type + "\"");
         HandleError(ex);
      }
      else if (!GmatStringUtil::IsValidName(type + desc, true))
      {
         // Check if function definition has been built
         if (inFunctionMode && !hasFunctionDefinition)
         {
            std::string funcStr = type + " = " + desc;
            if (type == "")
               funcStr = type + desc;
            
            if (BuildFunctionDefinition(funcStr))
            {
               retFlag = true;
               return NULL;
            }
         }
         
         InterpreterException ex
            ("Found invalid function name \"" + type + desc + "\"");
         HandleError(ex);
      }
      else
      {
         cmd = AppendCommand("CallFunction", retFlag, inCmd);
         desc1 = "[] =" + type + desc;
         cmd->SetGeneratingString(desc1);
      }
   }
   else
   {
      cmd = AppendCommand(type, retFlag, inCmd);
      cmd->SetGeneratingString(type + " " + desc);
   }
   
   if (cmd == NULL)
   {
      retFlag = false;
      return NULL;
   }
   
   
   #ifdef DEBUG_CREATE_COMMAND
   if (inCmd == NULL)
      MessageInterface::ShowMessage
         ("   => '%s' created.\n", cmd->GetTypeName().c_str());
   else
      MessageInterface::ShowMessage
         ("   => '%s' created and appended to '%s'.\n",
          cmd->GetTypeName().c_str(), inCmd->GetTypeName().c_str());
   #endif
   
   
   // Now assemble command
   try
   {
      // if command has its own InterpretAction(), just return cmd
      if (cmd->InterpretAction())
      {
         retFlag  = ValidateCommand(cmd);
         
         #ifdef DEBUG_CREATE_COMMAND
         MessageInterface::ShowMessage
            ("   ===> %s has InterpretAction() returning %p\n", type.c_str(), cmd);
         #endif
         
         return cmd;
      }
   }
   catch (BaseException &e)
   {
      HandleError(e);
      retFlag = false;
      // Return cmd since command already created
      return cmd;
   }
   
   if (desc1 != "")
   {
      bool retval3 = true;
      bool retval1  = AssembleCommand(cmd, desc1);
      if (retval1) retval3  = ValidateCommand(cmd);
      retFlag = retval1  && retval3;
   }
   
   return cmd;;
}


//------------------------------------------------------------------------------
//GmatCommand* AppendCommand(const std::string &type, bool &retFlag,
//                           GmatCommand *inCmd)
//------------------------------------------------------------------------------
GmatCommand* Interpreter::AppendCommand(const std::string &type, bool &retFlag,
                                        GmatCommand *inCmd)
{
   if (inCmd == NULL)
   {
      return theModerator->AppendCommand(type, "", retFlag);
   }
   else
   {
      GmatCommand *cmd = theModerator->CreateCommand(type, "", retFlag);
      inCmd->Append(cmd);
      
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage
         ("===> Interpreter::AppendCommand() inCmd=%s, cmd=%s\n",
          inCmd->GetTypeName().c_str(), cmd->GetTypeName().c_str());
      #endif
      
      return cmd;
   }
}


//------------------------------------------------------------------------------
//bool AssembleCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleCommand(GmatCommand *cmd, const std::string &desc)
{
   bool retval = false;
   std::string type = cmd->GetTypeName();

   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::AssembleCommand() cmd='%s'\n   desc=<%s>\n",
       type.c_str(), desc.c_str());
   #endif
   
   if (type == "For")
      retval = AssembleForCommand(cmd, desc);
   else if (type == "CallFunction")
      retval = AssembleCallFunctionCommand(cmd, desc);
   else if (cmd->IsOfType("ConditionalBranch"))
      retval = AssembleConditionalCommand(cmd, desc);
   else
      retval = AssembleGeneralCommand(cmd, desc);

   return retval;
}


//------------------------------------------------------------------------------
//bool AssembleCallFunctionCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleCallFunctionCommand(GmatCommand *cmd,
                                              const std::string &desc)
{
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage
      ("Interpreter::AssembleCallFunctionCommand() cmd='%s'\n   desc=<%s>\n",
       cmd->GetTypeName().c_str(), desc.c_str());
   #endif
   
   bool retval = true;
   
   // Output
   std::string::size_type index1 = desc.find("=");
   std::string lhs = desc.substr(0, index1);
   StringArray outArray;
   
   // get out parameters if there was an equal sign
   if (index1 != lhs.npos)
   {
      outArray = theTextParser.SeparateBrackets(lhs, "[]", " ,", true);
   }
   
   // Function Name, Input
   StringArray inArray;
   std::string funcName;
   std::string::size_type index2 = desc.find("(", index1);
   if (index2 == desc.npos)
   {
      funcName = desc.substr(index1+1);
   }
   else
   {
      funcName = desc.substr(index1+1, index2-index1-1);
      std::string rhs = desc.substr(index2);
      
      // Input
      inArray = theTextParser.SeparateBrackets(rhs, "()", " ,", true);
   }
   
   funcName = GmatStringUtil::Trim(funcName);
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage
      ("   Checking function name '%s'\n", funcName.c_str());
   #endif
   
   // Check for blank name
   if (funcName == "")
   {
      InterpreterException ex("*** ERROR *** Found blank function name");
      HandleError(ex);
      return false;
   }
   
   // Check for valid name
   if (!GmatStringUtil::IsValidName(funcName))
   {
      InterpreterException ex("*** ERROR *** Found invalid function name \"" + funcName + "\"");
      HandleError(ex);
      return false;
   }
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage("   Setting funcName '%s'\n", funcName.c_str());
   #endif
   
   // Set function name to CallFunction
   retval = cmd->SetStringParameter("FunctionName", funcName);
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage("   Setting input\n");
   WriteParts("CallFunction Input" , inArray);
   #endif
   
   // Set input to CallFunction
   bool validParam = false;
   if (inArray.size() == 0) //if no inputs, set validParam to true
      validParam = true;
   
   for (UnsignedInt i=0; i<inArray.size(); i++)
   {
      retval = cmd->SetStringParameter("AddInput", inArray[i]);
      
      // if input parameter is a system parameter then create (loj: 2007.11.16)
      validParam = false;      
      if (inArray[i].find('.') != std::string::npos)
      {
         if (IsParameterType(inArray[i]))
         {
            Parameter *param = CreateSystemParameter(inArray[i]);            
            if (param != NULL)
               validParam = true;
         }
      }
      else // whole object
      {
         GmatBase *obj = FindObject(inArray[i]);
         if (obj != NULL)
            validParam = true;
      }
      
      // if in function mode, ignore invalid parameter (loj: 2008.03.12)
      if (inFunctionMode)
         validParam = true;
      
      // if not in function mode, throw exception if invalid parameter
      if (!validParam)
      {
         InterpreterException ex
            ("Nonexistent or disallowed CallFunction Input Parameter: \"" +
             inArray[i] +  "\"");
         HandleError(ex);
      }
   }
   
   if (!retval || !validParam)
      return false;
   
   // Set output to CallFunction
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage("   Setting output\n");
   WriteParts("CallFunction Output" , outArray);
   #endif
   
   for (UnsignedInt i=0; i<outArray.size(); i++)
      retval = cmd->SetStringParameter("AddOutput", outArray[i]);
   
   // if in function mode, just return retval (loj: 2008.03.12)
   if (inFunctionMode)
      return retval;
   
   // See if Function is MatlabFunction since all MatlabFunctions are created
   // before mission sequence, if not, create as GmatFunction. (loj: 2008.03.12)
   // Changed to call FindObject() (loj: 2008.04.01)
   //GmatBase *func = GetConfiguredObject(funcName);
   GmatBase *func = FindObject(funcName);
   if (func == NULL)
      func = CreateObject("GmatFunction", funcName);
   
   // Set function pointer to CallFunction command
   cmd->SetRefObject(func, Gmat::FUNCTION, funcName);
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage
      ("Interpreter::AssembleCallFunctionCommand() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//bool AssembleConditionalCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleConditionalCommand(GmatCommand *cmd,
                                             const std::string &desc)
{
   bool retval = true;
   std::string type = cmd->GetTypeName();
   std::string opStr = "~<=>&|";
   
   // conditional commands, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc))
   {
      std::string msg = 
         "A conditional command is not allowed to contain brackets, braces, or "
         "parentheses (except to indicate an array element)";
      InterpreterException ex(msg);
      HandleError(ex);
      return false;
   }
   
   // This really becomes moot ...  wcs 2007.09.12
   // Remove enclosed parenthesis first
   Integer length = desc.size();
   std::string str1 = desc;
   if (desc[0] == '(' && desc[length-1] == ')')
   {
      str1 = desc.substr(1, length-2);
   }
   else
   {
      if (!GmatStringUtil::IsParenBalanced(desc))
      {
         InterpreterException ex("The Command has unbalanced parentheses");
         HandleError(ex);
         return false;
      }
   }
   
   std::string::size_type start = 0;
   std::string::size_type right = 0;
   std::string::size_type op = 0;
   bool done = false;
   StringArray parts;
   std::string str2;
   
   // Parse conditions
   while (!done)
   {
      op = str1.find_first_of(opStr, start);
      if (op == str1.npos)
      {
         // Add final right of operator, if not blank
         str2 = GmatStringUtil::Trim(str1.substr(start));
         if (str2 != "")
            parts.push_back(str2);
         break;
      }
      
      // Add left of operator
      str2 = GmatStringUtil::Trim(str1.substr(start, op-start));
      parts.push_back(str2);
      
      // Add operator
      right = str1.find_first_not_of(opStr, op);      
      str2 = GmatStringUtil::Trim(str1.substr(op, right-op));
      parts.push_back(str2);
      
      start = op + 1;
      op = str1.find_first_of(opStr, start);
      
      // check for double ops (such as: == ~= >= <=)
      if (op != str1.npos && op == start)
         start = op + 1;
   }
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   WriteParts("After parsing conditions()" , parts);
   #endif
   
   Integer count = parts.size();
   for (Integer ii = 0; ii < count; ii++)
   {
      if (GmatStringUtil::IsBlank(parts.at(ii)))
      {
         InterpreterException ex("Missing field or operator in command");
         HandleError(ex);
         return false;
      }
      std::string strUpper = GmatStringUtil::ToUpper(parts.at(ii));         
      if (strUpper.find(" OR ") != strUpper.npos)
      {
         InterpreterException ex("\"OR\" is not a valid relational operator");
         HandleError(ex);
         return false;
      }
      if (strUpper.find(" AND ") != strUpper.npos)
      {
         InterpreterException ex("\"AND\" is not a valid relational operator");
         HandleError(ex);
         return false;
      }
   }
   
   // assuming there is no boolean argument
   if (count < 3 || ((count-3)%4) != 0)
   {
      InterpreterException ex("The Command has an invalid number of conditions");
      HandleError(ex);
      return false;
   }
   
   ConditionalBranch *cb = (ConditionalBranch*)cmd;
   
   for (int i=0; i<count; i+=4)
   {
      #ifdef DEBUG_ASSEMBLE_COMMAND
      MessageInterface::ShowMessage
         ("   lhs:<%s>, op:<%s>, rhs:<%s>\n", parts[i].c_str(), parts[i+1].c_str(),
          parts[i+2].c_str());
      #endif
      
      // Try to create a parameter first if system parameter
      std::string type, ownerName, depObj;
      GmatStringUtil::ParseParameter(parts[i], type, ownerName, depObj);
      #ifdef DEBUG_ASSEMBLE_COMMAND // --------------------------------- debug ----
      MessageInterface::ShowMessage
         ("   lhs: type = %s, ownerName = %s, depObj = %s\n", 
         type.c_str(), ownerName.c_str(), depObj.c_str());
      #endif // ------------------------------------------------- end debug ----
      
      if (theModerator->IsParameter(type))
          CreateParameter(type, parts[i], ownerName, depObj);
      
      GmatStringUtil::ParseParameter(parts[i+2], type, ownerName, depObj);
      #ifdef DEBUG_ASSEMBLE_COMMAND // --------------------------------- debug ----
      MessageInterface::ShowMessage
         ("   rhs: type = %s, ownerName = %s, depObj = %s\n", 
         type.c_str(), ownerName.c_str(), depObj.c_str());
      #endif // ------------------------------------------------- end debug ----
      
      if (theModerator->IsParameter(type))
         CreateParameter(type, parts[i+2], ownerName, depObj);
      
      cb->SetCondition(parts[i], parts[i+1], parts[i+2]);
      
      if (count > i+3)
      {
         #ifdef DEBUG_ASSEMBLE_COMMAND
         MessageInterface::ShowMessage("   logOp=<%s>\n", parts[i+3].c_str());
         #endif
         
         cb->SetConditionOperator(parts[i+3]);
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
//bool AssembleForCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
/* Parses For loop control expression
 *    It's syntax is 
 *       For index = start:increment:end
 */
//------------------------------------------------------------------------------
bool Interpreter::AssembleForCommand(GmatCommand *cmd, const std::string &desc)
{
   #ifdef DEBUG_ASSEMBLE_FOR
   MessageInterface::ShowMessage
      ("Interpreter::AssembleForCommand() desc=<%s>\n", desc.c_str());
   #endif
   
   // For loop commands, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc))
   {
      std::string msg = 
         "A For command is not allowed to contain brackets, braces, or "
         "parentheses (except to indicate an array element)";
      InterpreterException ex(msg);
      HandleError(ex);
      return false;
   }
   
   bool retval = true;
   std::string::size_type equalSign = desc.find("=");
   
   if (equalSign == desc.npos)
   {
      InterpreterException ex("Cannot find equal sign (=) for For loop control");
      HandleError(ex);
      return false;
   }
   
   std::string index = desc.substr(0, equalSign);
   index = GmatStringUtil::Trim(index);
   
   std::string substr = desc.substr(equalSign+1);
   if (substr.find(':') == substr.npos)
   {
      InterpreterException ex("Missing colon (:) for For loop control");
      HandleError(ex);
      return false;
   }
   
   StringArray parts = theTextParser.SeparateBy(substr, ":");
   int count = parts.size();
   Integer numColons = 0;
   for (unsigned int ii = 0; ii < substr.size(); ii++)
      if (substr.at(ii) == ':') numColons++;
   if (numColons >= (Integer) count)
   {
      InterpreterException ex("Too many colons (:) for For loop control");
      HandleError(ex);
      return false;
   }
   #ifdef DEBUG_ASSEMBLE_FOR
   MessageInterface::ShowMessage
      ("Interpreter::AssembleForCommand() After SeparateBy, parts = \n");
   for (Integer ii=0;ii<count;ii++)
      MessageInterface::ShowMessage("   <%s>\n", parts[ii].c_str());
   #endif
   
   if (count < 2)
   {
      InterpreterException ex("Missing field, colon (:), or equal sign (=) for For loop control");
      HandleError(ex);
      return false;
   }
   
   std::string start = parts[0];
   std::string end = parts[1];
   std::string step = "1";
   
   if (count > 2)
   {
      step = parts[1];
      end = parts[2];
   }
   
   
   #ifdef DEBUG_ASSEMBLE_FOR
   MessageInterface::ShowMessage
      ("Interpreter::AssembleForCommand() index=<%s>, start=<%s>, end=<%s>, "
       "step=<%s>\n", index.c_str(), start.c_str(), end.c_str(), step.c_str());
   #endif
   
   cmd->SetStringParameter("IndexName", index);
   cmd->SetStringParameter("StartName", start);
   cmd->SetStringParameter("EndName", end);
   cmd->SetStringParameter("IncrementName", step);
   
   #ifdef DEBUG_ASSEMBLE_FOR
   MessageInterface::ShowMessage
      ("Interpreter::AssembleForCommand() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//bool AssembleGeneralCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleGeneralCommand(GmatCommand *cmd,
                                         const std::string &desc)
{
   bool retval = true;
   std::string type = cmd->GetTypeName();
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("AssembleGeneralCommand() cmd='%s', desc=<%s>\n", cmd->GetTypeName().c_str(),
       desc.c_str());
   #endif
   
   if (type == "Target" || type == "Report" || type == "BeginFiniteBurn" ||
       type == "EndFiniteBurn" || type == "Optimize")
   {
      // first item is ref. object name
      
      if (type == "Target")
      {
         AssembleTargetCommand(cmd, desc);
      }
      else if (type == "Optimize")
      {
         AssembleOptimizeCommand(cmd, desc);
      }
      else if (type == "Report")
      {
         return AssembleReportCommand(cmd, desc);
      }
      else
      {
         return AssembleFiniteBurnCommand(cmd, desc);
      }
   }
   else if (type == "Create")
   {
      retval = AssembleCreateCommand(cmd, desc);
   }
   else if (type == "Save" || type == "Global")
   {
      retval = SetCommandRefObjects(cmd, desc);
   }
   else
   {
      retval = false;
   }
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("AssembleGeneralCommand() leaving setting %s, retval=%d\n",
       type.c_str(), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool AssembleTargetCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleTargetCommand(GmatCommand *cmd, const std::string &desc)
{
   // This command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc, false))
   {
      std::string msg = 
         "The Target command is not allowed to contain brackets, braces, or "
         "parentheses";
      InterpreterException ex(msg);
      HandleError(ex);
      return false;
   }
   
   bool retval = true;
   StringArray parts = theTextParser.Decompose(desc, "()");
   cmd->SetRefObjectName(Gmat::SOLVER, parts[0]);
   
   // Make sure there is only one thing on the line
   if (parts.size() > 1)
   {
      InterpreterException ex
         ("Unexpected text at end of Target command");
      HandleError(ex);
      retval = false;
   }
   
   // Check if the Solver exist if not in Function mode
   if (!inFunctionMode)
   {
      GmatBase *obj = FindObject(parts[0], "Solver");
      if (obj == NULL)
      {
         InterpreterException ex
            ("Cannot find the Solver \"" + parts[0] + "\"");
         HandleError(ex);
         retval = false;
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool AssembleOptimizeCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleOptimizeCommand(GmatCommand *cmd, const std::string &desc)
{
   // This command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc, false))
   {
      std::string msg = 
         "The Optimize command is not allowed to contain brackets, braces, or "
         "parentheses";
      InterpreterException ex(msg);
      HandleError(ex);
      return false;
   }
   
   bool retval = true;
   StringArray parts = theTextParser.Decompose(desc, "()");
   cmd->SetRefObjectName(Gmat::SOLVER, parts[0]);
   
   // Make sure there is only one thing on the line
   if (parts.size() > 1)
   {
      InterpreterException ex
         ("Unexpected text at end of Optimize command");
      HandleError(ex);
      retval = false;
   }
   
   // Check if the Solver exist if not in Function mode
   if (!inFunctionMode)
   {
      GmatBase *obj = FindObject(parts[0], "Solver");
      if (obj == NULL)
      {
         InterpreterException ex
            ("Cannot find the Solver \"" + parts[0] + "\"");
         HandleError(ex);
         retval = false;
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool AssembleFiniteBurnCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleFiniteBurnCommand(GmatCommand *cmd, const std::string &desc)
{
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage("Begin/EndFiniteBurn being processed ...");
   #endif
   
   bool retval = true;
   
   // Note:
   // Begin/EndFiniteBurn has the syntax: BeginFiniteBurn burn1(sat1 sat2)
   // First, check for errors in brackets
   if ((desc.find("[") != desc.npos) || (desc.find("]") != desc.npos))
   {
      InterpreterException ex
         ("Brackets not allowed in " + cmd->GetTypeName()+ " command");
      HandleError(ex);
      retval = false;
   }
   
   if (!GmatStringUtil::AreAllBracketsBalanced(desc, "({)}"))
   {
      InterpreterException ex
         ("Parentheses, braces, or brackets are unbalanced or incorrectly placed");
      HandleError(ex);
      retval = false;
   }
   
   // Get FiniteBurn name
   StringArray parts = theTextParser.Decompose(desc, "()", false);
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   std::string type = cmd->GetTypeName();
   WriteParts(type, parts);
   #endif
   
   if (parts.size() < 2)
   {
      InterpreterException ex
         ("Missing " + cmd->GetTypeName() + " parameter. Expecting "
          "\"FiniteBurnName(SpacecraftName)\"");
      HandleError(ex);
      retval = false;
   }
   else
   {
      cmd->SetRefObjectName(Gmat::FINITE_BURN, parts[0]);
      
      // Get Spacecraft names
      StringArray subParts = theTextParser.SeparateBrackets(parts[1], "()", ",");
      
      #ifdef DEBUG_ASSEMBLE_COMMAND
      WriteParts(type, subParts);
      #endif
      
      Integer count = subParts.size();
      if (count == 0)
      {
         InterpreterException ex
            (cmd->GetTypeName() + " command must contain at least one spacecraft name");
         HandleError(ex);
         retval = false;
      }
      Integer numCommas = GmatStringUtil::NumberOfOccurrences(parts[1],',');
      if (count != (numCommas + 1))
      {
         InterpreterException ex
            ("Missing spacecraft name in " + cmd->GetTypeName() + " command");
         HandleError(ex);
         retval = false;
      }
      for (int i=0; i<count; i++)
      {
         if (GmatStringUtil::IsBlank(subParts[i]))
         {
            InterpreterException ex
               ("Missing spacecraft name in " + cmd->GetTypeName() + " command");
            HandleError(ex);
            retval = false;
         }
         cmd->SetRefObjectName(Gmat::SPACECRAFT, subParts[i]);
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool AssembleReportCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleReportCommand(GmatCommand *cmd, const std::string &desc)
{
   #ifdef DEBUG_ASSEMBLE_REPORT_COMMAND
   MessageInterface::ShowMessage
      ("AssembleReportCommand() cmd='%s', desc=<%s>\n", cmd->GetTypeName().c_str(),
       desc.c_str());
   #endif
   
   bool retval = true;
   
   // This command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc, true))
   {
      std::string msg = 
         "The Report command is not allowed to contain brackets, braces, or "
         "parentheses (except to indicate array elements)";
      InterpreterException ex(msg);
      HandleError(ex);
      return false;
   }
   
   // we only want to separate by spaces - commas are not allowed, 
   // not even in arrays (for this command)
   StringArray parts = GmatStringUtil::SeparateBy(desc, " ", true);
   Integer count = parts.size();
   
   #ifdef DEBUG_ASSEMBLE_REPORT_COMMAND 
   WriteParts("Parsing Report, parts:", parts);
   #endif
   
   // We can skip checking for configured object if in Function mode
   if (!inFunctionMode)
   {
      // first item is ReportFile
      GmatBase *obj = FindObject(parts[0]);
      
      if (obj != NULL)
      {
         cmd->SetRefObject(obj, Gmat::SUBSCRIBER, parts[0], 0);
         
         // Check if the Report exists
         GmatBase *obj = FindObject(parts[0], "ReportFile");
         if (obj == NULL)
         {
            InterpreterException ex
               ("Cannot find the ReportFile \"" + parts[0] + "\"");
            HandleError(ex);
            retval = false;
         }
         
         // checking items to report
         if (count < 2)
         {
            InterpreterException ex ("There are no items to report");
            HandleError(ex);
            retval = false;
         }
         else
         {
            // next items are Parameters
            for (int i=1; i<count; i++)
            {
               obj = (GmatBase*)CreateSystemParameter(parts[i]);
               if (obj != NULL)
               {
                  cmd->SetRefObject(obj, Gmat::PARAMETER, parts[i], 0);
               }
               else
               {
                  InterpreterException ex
                     ("Nonexistent or disallowed Report Variable: \"" + parts[i] +
                      "\";\nIt will not be added to Report");
                  HandleError(ex);
                  retval = false;
               }
            }
         }
      }
      else
      {
         InterpreterException ex
            ("Cannot find the ReportFile \"" + parts[0] + "\"");
         HandleError(ex);
         retval = false;
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool AssembleCreateCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleCreateCommand(GmatCommand *cmd, const std::string &desc)
{   
   #ifdef DEBUG_ASSEMBLE_CREATE
   MessageInterface::ShowMessage
      ("AssembleCreateCommand() Create command desc=<%s>\n", desc.c_str());
   #endif
   
   std::string::size_type typeIndex = desc.find_first_of(" ");
   std::string objTypeStr = desc.substr(0, typeIndex);
   std::string objNameStr = desc.substr(typeIndex+1);
   
   #ifdef DEBUG_ASSEMBLE_CREATE
   MessageInterface::ShowMessage("   Create object type=<%s>\n", objTypeStr.c_str());
   MessageInterface::ShowMessage("   Create object name=<%s>\n", objNameStr.c_str());
   #endif
   
   // check if object type is valid
   if (!IsObjectType(objTypeStr))
   {
      InterpreterException ex
         ("Unknown object type \"" + objTypeStr + "\" found in " +
          cmd->GetTypeName() + " command");
      HandleError(ex);
      return false;
   }
   
   StringArray parts = GmatStringUtil::SeparateBy(objNameStr, " ", true);
   
   #ifdef DEBUG_ASSEMBLE_CREATE
   WriteParts("Create object name parts", parts);
   #endif
   
   if (parts.size() == 0)
   {
      InterpreterException ex
         ("Missing object name found in " + cmd->GetTypeName() + " command");
      HandleError(ex);
      return false;
   }
   
   try
   {
      cmd->SetStringParameter("ObjectType", objTypeStr);
      for (UnsignedInt i=0; i<parts.size(); i++)
         cmd->SetStringParameter("ObjectNames", parts[i]);
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
      throw;
   }
   
   //-------------------------------------------------------------------
   // Create an unmanaged object and set to command
   // Note: Generally unnamed object will not be added to configuration,
   //       but we need name for Array for syntax checking, so pass name
   //       and set false to unmanage Array objects
   //-------------------------------------------------------------------
   std::string name;
   if (objTypeStr == "Array")
      name = parts[0];
   
   // Special case for Propagator
   if (objTypeStr == "Propagator")
      objTypeStr = "PropSetup";
   
   GmatBase *obj = CreateObject(objTypeStr, name, false);
   
   if (obj == NULL)
      return false;
   
   #ifdef DEBUG_ASSEMBLE_CREATE
   MessageInterface::ShowMessage
      ("   obj=<%p>, objType=<%s>, objName=<%s> created\n",
       obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
      
   return true;
}


//------------------------------------------------------------------------------
// bool SetCommandRefObjects(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::SetCommandRefObjects(GmatCommand *cmd, const std::string &desc)
{
   #ifdef DEBUG_ASSEMBLE_COMMAND   
   MessageInterface::ShowMessage
      ("Interpreter::SetCommandRefObjects() cmd=<%s>, desc=<%s>\n",
       cmd->GetTypeName().c_str(), desc.c_str());
   #endif
   
   // Save, Global commands, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces.
   // Since Create command can have "Create Array vec[3,1]", so do not check.
   if (!GmatStringUtil::HasNoBrackets(desc, false))
   {
      std::string msg = 
         "The " + cmd->GetTypeName() + " command is not allowed to contain "
         "brackets, braces, or parentheses";
      InterpreterException ex(msg);
      HandleError(ex);
      return false;
   }
   
   // we only want to separate by spaces - commas are not allowed, 
   // not even in arrays (for this command)
   StringArray parts = GmatStringUtil::SeparateBy(desc, " ", true);
   unsigned int numParts = parts.size();
   bool isOk = true;
   
   if (numParts == 0)
   {
      std::string msg = 
         "The " + cmd->GetTypeName() + " command has missing object names";
      InterpreterException ex(msg);
      HandleError(ex);
      return false;
   }
   
   #ifdef DEBUG_ASSEMBLE_COMMAND   
   WriteParts("object name parts", parts);
   #endif
   
   for (unsigned int i=0; i<numParts; i++)
   {
      if (parts[i].find(',') != parts[i].npos)
      {
         std::string msg = 
            "The " + cmd->GetTypeName() + " command is not allowed to contain commas - "
            "separate objects by spaces";
         InterpreterException ex(msg);
         HandleError(ex);
         isOk = false;
      }
      else if (!GmatStringUtil::IsValidName(parts[i]))
      {
         std::string msg = 
            "\"" + parts[i] + "\" is an invalid object name in " +
            cmd->GetTypeName() + " command";
         InterpreterException ex(msg);
         HandleError(ex);
         isOk = false;
      }
      else
      {
         cmd->SetStringParameter("ObjectNames", parts[i]);
      }
   }
   
   return isOk;
}


//------------------------------------------------------------------------------
//GmatCommand* CreateAssignmentCommand(const std::string &lhs,
//                                     const std::string &rhs, bool &retFlag,
//                                     GmatCommand *inCmd)
//------------------------------------------------------------------------------
GmatCommand* Interpreter::CreateAssignmentCommand(const std::string &lhs,
                                                  const std::string &rhs,
                                                  bool &retFlag, GmatCommand *inCmd)
{
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::CreateAssignmentCommand() lhs=<%s>, rhs=<%s>\n", lhs.c_str(),
       rhs.c_str());
   #endif
   
   // First check if it is really assignment by checking blank in the lhs.
   // (The lhs must be Variable, String, Array, or object property and this is
   //  validated in the Assignment command)
   std::string::size_type index = lhs.find_last_of(" ");
   if (index != lhs.npos)
   {
      std::string cmd = lhs.substr(0, index);
      
      // See if it is an Array since array index can have blanks
      index = lhs.find("(");
      if (index != lhs.npos)
      {
         if (!IsArrayElement(lhs))
         {
            InterpreterException ex("\"" + cmd + "\" is not a valid Command");
            HandleError(ex);
            return NULL;
         }
      }
   }
   
   std::string desc = lhs + " = " + rhs;
   return CreateCommand("GMAT", desc, retFlag, inCmd);
}


//------------------------------------------------------------------------------
// Parameter* CreateSystemParameter(const std::string &name, bool manage = true)
//------------------------------------------------------------------------------
/**
 * Creates a system Parameter from the input parameter name. If the name contains
 * dots, it consider it as a system parameter.  If it is not a system Parameter
 * it checks if object by given name is a Parameter.
 *
 * @param  name   parameter name to be parsed for Parameter creation
 *                Such as, sat1.Earth.ECC, sat1.SMA
 * @param  manage true if parameter to be managed in the configuration (true)
 *
 * @return Created Paramteter pointer or pointer of the Parameter by given name
 *         NULL if it is not a system Parameter nor named object is not a Parameter
 *
 */
//------------------------------------------------------------------------------
Parameter* Interpreter::CreateSystemParameter(const std::string &str,
                                              bool manage)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Interpreter::CreateSystemParameter() str=%s\n", str.c_str());
   #endif
   
   Parameter *param = NULL;
   std::string paramType, ownerName, depName;
   GmatStringUtil::ParseParameter(str, paramType, ownerName, depName);
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("   paramType=%s, ownerName=%s, depName=%s\n", paramType.c_str(),
       ownerName.c_str(), depName.c_str());
   #endif
   
   // Create parameter, if type is a System Parameter
   if (find(parameterList.begin(), parameterList.end(), paramType) != 
       parameterList.end())
   {
      param = CreateParameter(paramType, str, ownerName, depName, manage);
      
      #ifdef DEBUG_CREATE_PARAM
      MessageInterface::ShowMessage
         ("   Parameter created with paramType=%s, ownerName=%s, depName=%s\n",
          paramType.c_str(), ownerName.c_str(), depName.c_str());
      #endif
   }
   else
   {
      // Find the object and check if it is a Parameter
      GmatBase *obj = FindObject(str);
      if (obj != NULL && obj->GetType() == Gmat::PARAMETER)
      {
         param = (Parameter*)obj;
      
         #ifdef DEBUG_CREATE_PARAM
         MessageInterface::ShowMessage("   Parameter <%s> found\n", str.c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Interpreter::CreateSystemParameter() returning <%p><%s><%s>\n", param,
       (param == NULL) ? "NULL" : param->GetTypeName().c_str(),
       (param == NULL) ? "NULL" : param->GetName().c_str());
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name,
//                            const std::string &ownerName, const std::string &depName
//                            bool manage = true)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Parameter.
 * 
 * @param  type       Type of parameter requested
 * @param  name       Name for the parameter.
 * @param  ownerName  object name of parameter requested ("")
 * @param  depName    Dependent object name of parameter requested ("")
 * @param  manage     true if created object to be added to configuration (true)
 * 
 * @return Pointer to the constructed Parameter.
 */
//------------------------------------------------------------------------------
Parameter* Interpreter::CreateParameter(const std::string &type, 
                                        const std::string &name,
                                        const std::string &ownerName,
                                        const std::string &depName,
                                        bool manage)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Interpreter::CreateParameter() type='%s', name='%s', ownerName='%s', "
       "depName='%s', manage=%d\n", type.c_str(), name.c_str(),
       ownerName.c_str(), depName.c_str(), manage);
   #endif
   
   // Check if create an array
   if (type == "Array")
      return CreateArray(name, manage);
   else
      return theModerator->CreateParameter(type, name, ownerName, depName, manage);
}


//------------------------------------------------------------------------------
// Parameter* CreateArray(onst std::string &arrayStr, bool manage = true)
//------------------------------------------------------------------------------
Parameter* Interpreter::CreateArray(const std::string &arrayStr, bool manage)
{
   std::string name;
   Integer row, col;
   GmatStringUtil::GetArrayIndex(arrayStr, row, col, name, "[]");
   
   if (row == -1)
   {
      InterpreterException ex
         ("Interpreter::CreateArray() invalid number of rows found in: " +
          arrayStr + "\n");
      HandleError(ex);
   }
   
   if (col == -1)
   {
      InterpreterException ex
         ("Interpreter::CreateArray() invalid number of columns found in: " +
          arrayStr + "\n");
      HandleError(ex);
   }
   
   Parameter *param = theModerator->CreateParameter("Array", name, "", "", manage);
   
   #ifdef DEBUG_CREATE_ARRAY
   MessageInterface::ShowMessage
      ("Interpreter::CreateArray() name='%s', row=%d, col=%d\n", name.c_str(), row, col);
   #endif
   
   ((Array*)param)->SetSize(row, col);
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* GetArrayIndex(const std::string &arrayStr, Integer &row, Integer &col)
//------------------------------------------------------------------------------
/**
 * Retrives array index from the configured array.
 *
 * @param  arrayStr  String form of array (A(1,3), B(2,j), etc)
 *
 * @note Array name must be created and configured before acces.
 */
//------------------------------------------------------------------------------
Parameter* Interpreter::GetArrayIndex(const std::string &arrayStr,
                                      Integer &row, Integer &col)
{
   std::string name, rowStr, colStr;
   
   // parse array name and index
   GmatStringUtil::GetArrayIndex(arrayStr, rowStr, colStr, row, col, name);

   // Remove - sign from the name
   if (name[0] == '-')
      name = name.substr(1);
   
   #ifdef DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("Interpreter::GetArrayIndex() arrayStr=<%s>, name=<%s>, rowStr=<%s>, "
       "colStr=<%s>, row=%d, col=%d\n", arrayStr.c_str(), name.c_str(),
       rowStr.c_str(), colStr.c_str(), row, col);
   #endif

   Parameter *param = (Parameter*)FindObject(name);

   // Note:
   // To catch errors as much as possible, limited return statement used
   // even when error found
   
   if (param == NULL)
   {
      InterpreterException ex("Array named \"" + name + "\" is undefined");
      HandleError(ex);
   }
   else
   {
      if (param->GetTypeName() != "Array")
      {
         InterpreterException ex("\"" + name + " is not an Array");
         HandleError(ex);
      }
      
      if (rowStr == "0" || colStr == "0" ||rowStr == "-1" || colStr == "-1")
      {
         InterpreterException ex("Index exceeds matrix dimensions");
         HandleError(ex);
         return NULL;
      }
      
      // get row value
      if (row == -1 && rowStr != "-1")
      {
         Parameter *rowParam = (Parameter*)FindObject(rowStr);
         if (rowParam == NULL)
         {
            InterpreterException ex
               ("Array row index named \"" + rowStr + "\" is undefined");
            HandleError(ex);
         }
         else
         {
            if (rowParam->GetReturnType() == Gmat::REAL_TYPE)
            {
               row = (Integer)rowParam->GetReal() - 1; // index starts at 0
            }
            else
            {
               InterpreterException ex
                  ("Cannot handle row index of Array named \"" + name + "\"");
               HandleError(ex);
            }
         }
      }
      
      // get column value
      if (col == -1 && colStr != "-1")
      {
         Parameter *colParam = (Parameter*)FindObject(colStr);
         if (colParam == NULL)
         {
            InterpreterException ex
               ("Column index named \"" + colStr + "\" is undefined");
            HandleError(ex);
         }
         else
         {
            if (colParam->GetReturnType() == Gmat::REAL_TYPE)
            {
               col = (Integer)colParam->GetReal() - 1; // index starts at 0
            }
            else
            {
               InterpreterException ex
                  ("Cannot handle column index of Array named \"" + name + "\"");
               HandleError(ex);
            }
         }
      }
   }
   
   #ifdef DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("   GetArrayIndex() row=%d, col=%d\n", row, col);
   #endif
   
   if (param == NULL || row == -1 || col == -1)
      return NULL;
   else
      return param;
}


//------------------------------------------------------------------------------
// GmatBase* MakeAssignment(const std::string &lhs, const std::string &rhs)
//------------------------------------------------------------------------------
/*
 * Sets rhs to lhs.
 *
 * @param  lhs  Left hand side component
 * @param  rhs  Right hand side component
 *
 * @return return LHS object pointer
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::MakeAssignment(const std::string &lhs, const std::string &rhs)
{
   #ifdef DEBUG_ASSIGNMENT
   MessageInterface::ShowMessage
      ("Interpreter::MakeAssignment() lhs=<%s>, rhs=<%s>\n", lhs.c_str(), rhs.c_str());
   MessageInterface::ShowMessage
      ("   inFunctionMode=%d, hasFunctionDefinition=%d\n", inFunctionMode,
       hasFunctionDefinition);
   #endif
   
   // Check if it is function definition line
   if (inFunctionMode && !hasFunctionDefinition)
   {
      MessageInterface::ShowMessage("   ==> check function definition\n");
      if (BuildFunctionDefinition(lhs + " = " + rhs))
         return NULL;
   }
   
   bool retval = false;
   
   // Separate dots
   StringArray lhsParts = theTextParser.SeparateDots(lhs);
   Integer lhsPartCount = lhsParts.size();
   StringArray rhsParts = theTextParser.SeparateDots(rhs);
   Integer rhsPartCount = rhsParts.size();
   std::string::size_type dot;
   std::string lhsObjName, rhsObjName;
   std::string lhsPropName, rhsPropName;
   GmatBase *lhsObj = NULL;
   GmatBase *rhsObj = NULL;
   bool isLhsObject = false;
   bool isRhsObject = false;
   bool isLhsArray = false;
   bool isRhsArray = false;
   currentBlock = lhs + " = " + rhs;
   
   #ifdef DEBUG_ASSIGNMENT
   MessageInterface::ShowMessage
      ("   lhsPartCount=%d, rhsPartCount=%d\n", lhsPartCount, rhsPartCount);
   #endif
   
   // check LHS
   if (lhsPartCount > 1)
   {
      lhsObjName = lhsParts[0];
      lhsObj = FindObject(lhsObjName);
      
      if (lhsObj == NULL)
      {
         if (lhs == "")
         {
            InterpreterException ex("Object field assignment is imcomplelte");
            HandleError(ex);
         }
         else
         {
            InterpreterException ex
               ("Cannot find LHS object named \"" + lhsObjName + "\"");
            HandleError(ex);
         }
         return NULL;
      }
      
      dot = lhs.find('.');
      if (dot == lhs.npos)
         lhsPropName = lhsParts[1];
      else
         lhsPropName = lhs.substr(dot+1);
      
   }
   else
   {
      lhsObj = FindObject(lhs);
      
      if (lhsObj)
      {
         if (IsArrayElement(lhs))
            isLhsArray = true;
         else
            isLhsObject = true;
      }
      else
      {
         if (lhs == "")
         {
            InterpreterException ex("Missing equal sign in object field assignment");
            HandleError(ex);
         }
         else
         {
            InterpreterException ex("Cannot find LHS object named \"" + lhs + "\"");
            HandleError(ex);
         }
         return NULL;
      }
   }
   
   #ifdef DEBUG_ASSIGNMENT
   MessageInterface::ShowMessage
      ("   isLhsObject=%d, isLhsArray=%d, lhsPropName=<%s>, lhsObj=<%p><%s>\n",
       isLhsObject, isLhsArray, lhsPropName.c_str(), lhsObj,
       (lhsObj == NULL) ? "NULL" : lhsObj->GetName().c_str() );
   #endif
   
   // check RHS
   if (rhsPartCount > 1)
   {
      rhsObjName = rhsParts[0];
      rhsObj = FindObject(rhsObjName);
      
      if (rhsObj == NULL)
      {
         //throw InterpreterException("Cannot find RHS object: " + rhsObjName + "\n");
         
         #ifdef DEBUG_ASSIGNMENT
         MessageInterface::ShowMessage
            ("   Cannot find RHS object: '%s'. It may be a string value\n",
             rhsObjName.c_str());
         #endif
      }
      else
      {
         // Note: Do not set rhsObj to true here since it needs to create
         // a Parameter if needed.
         
         dot = rhs.find('.');
         if (dot == rhs.npos)
         {
            rhsPropName = rhsParts[1];
         }
         else
         {
            // check if it is property first
            std::string afterDot = rhs.substr(dot+1);
            GmatBase *toObj = NULL;
            Integer toId = -1;
            Gmat::ParameterType toType;
            if (FindPropertyID(rhsObj, afterDot, &toObj, toId, toType))
               rhsPropName = afterDot;
            else
               rhsPropName = rhsParts[1];
         }
      }
   }
   else
   {
      // If firist RHS char is "-" sign, use without it in finding name.
      // This is due to backward propagation. For example,
      // Propagate -prop(Sat1, Sat2, {Sat1.Periapsis})
      std::string newName = rhs;
      
      if (rhs[0] == '-')
         newName = rhs.substr(1);
      
      rhsObj = FindObject(newName);
      
      if (rhsObj)
      {
         if (IsArrayElement(rhs))
            isRhsArray = true;
         else
         {
            // Note: loj: 12/07/06
            // We want to allow user to create object and name it with one of
            // ObjectTypes. e.g. Create Spacecraft Spacecraft.
            // So if name found in configuration and not an ObjectType, except
            // calculated PARAMETER, it will considered as string value.
            if (IsObjectType(newName) && rhsObj->GetType() != Gmat::PARAMETER)
               isRhsObject = false;
            else
               isRhsObject = true;
         }
      }
   }
   
   #ifdef DEBUG_ASSIGNMENT
   MessageInterface::ShowMessage
      ("   isRhsObject=%d, isRhsArray=%d, rhsPropName=<%s>, rhsObj=<%p><%s>\n",
       isRhsObject, isRhsArray, rhsPropName.c_str(), rhsObj,
       (rhsObj == NULL) ? "NULL" : rhsObj->GetName().c_str() );
   #endif
   
   if (isLhsObject)
   {
      if (isRhsObject)
         retval = SetObjectToObject(lhsObj, rhsObj);
      else if (rhsPropName != "")
         retval = SetPropertyToObject(lhsObj, rhsObj, rhsPropName);
      else if (isRhsArray)
         retval = SetArrayToObject(lhsObj, rhs);
      else
         retval = SetValueToObject(lhsObj, rhs);
   }
   else if (lhsPropName != "")
   {
      if (isRhsObject)
         retval = SetObjectToProperty(lhsObj, lhsPropName, rhsObj);
      else if (rhsPropName != "")
         retval = SetPropertyToProperty(lhsObj, lhsPropName, rhsObj, rhsPropName);
      else if (isRhsArray)
         retval = SetArrayToProperty(lhsObj, lhsPropName, rhs);
      else
         retval = SetValueToProperty(lhsObj, lhsPropName, rhs);
   }
   else if (isLhsArray)
   {
      if (isRhsObject)
         retval = SetObjectToArray(lhsObj, lhs, rhsObj);
      else if (rhsPropName != "")
         retval = SetPropertyToArray(lhsObj, lhs, rhsObj, rhsPropName);
      else if (isRhsArray)
         retval = SetArrayToArray(lhsObj, lhs, rhsObj, rhs);
      else
         retval = SetValueToArray(lhsObj, lhs, rhs);
   }
   else
   {
      InterpreterException ex
         ("Interpreter::MakeAssignment() Internal error if it reached here.");
      HandleError(ex);
   }
   
   #ifdef DEBUG_ASSIGNMENT
   MessageInterface::ShowMessage
      ("Interpreter::MakeAssignment() returning lhsObj=%p\n", lhsObj);
   #endif
   
   if (retval)
      return lhsObj;
   else
      return NULL;
}


//-------------------------------------------------------------------------------
// bool SetObjectToObject(GmatBase *toObj, GmatBase *fromObj)
//-------------------------------------------------------------------------------
bool Interpreter::SetObjectToObject(GmatBase *toObj, GmatBase *fromObj)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToObject() to=%s, from=%s\n",
       toObj->GetName().c_str(), fromObj->GetName().c_str());
   #endif
   
   // Copy object
   if (toObj->GetTypeName() == fromObj->GetTypeName())
   {
      toObj->Copy(fromObj);
   }
   else
   {
      InterpreterException ex("Object type of LHS and RHS are not the same.");
      HandleError(ex);
      return false;
   }
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetPropertyToObject(GmatBase *toObj, GmatBase *fromOwner,
//                          const std::string &fromProp)
//-------------------------------------------------------------------------------
bool Interpreter::SetPropertyToObject(GmatBase *toObj, GmatBase *fromOwner,
                                      const std::string &fromProp)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetPropertyToObject() toObj=%s, fromOwner=%s, fromProp=%s\n",
       toObj->GetName().c_str(), fromOwner->GetName().c_str(), fromProp.c_str());
   #endif
   
   std::string rhs = fromOwner->GetName() + "." + fromProp;
   Integer fromId = -1;
   Gmat::ParameterType fromType = Gmat::UNKNOWN_PARAMETER_TYPE;
   Parameter *rhsParam = NULL;
   
   if (toObj->GetTypeName() != "Variable" && toObj->GetTypeName() != "String")
   {
      InterpreterException ex
         ("Setting \"" + fromProp + "\" to an object \"" + toObj->GetName() +
          "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   try
   {
      fromId = fromOwner->GetParameterID(fromProp);
      fromType = fromOwner->GetParameterType(fromId);
   }
   catch (BaseException &e)
   {
      // try if fromProp is a system Parameter
      rhsParam = CreateSystemParameter(rhs);
      
      if (rhsParam == NULL)      //toType = toOwner->GetParameterType(toId);

      {
         HandleError(e);
         return false;
      }
      
      fromType = rhsParam->GetReturnType();
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("SetPropertyToObject() rhs:%s is a parameter\n", rhs.c_str());
      #endif
   }
   
   Gmat::ParameterType toType = ((Parameter*)toObj)->GetReturnType();
   
   if (fromType == toType)
   {
      if (fromId == -1)
      {
         if (toType == Gmat::STRING_TYPE || toType == Gmat::ENUMERATION_TYPE)
            toObj->SetStringParameter("Value", rhsParam->GetString());
         else if (toType == Gmat::REAL_TYPE)
            toObj->SetRealParameter("Value", rhsParam->EvaluateReal());
      }
      else
      {
         if (toType == Gmat::STRING_TYPE || toType == Gmat::ENUMERATION_TYPE)
            toObj->SetStringParameter("Value", fromOwner->GetStringParameter(fromId));
         else if (toType == Gmat::REAL_TYPE)
            toObj->SetRealParameter("Value", fromOwner->GetRealParameter(fromId));
      }
   }
   else
   {
      InterpreterException ex
         ("Setting \"" + fromProp + "\" to an object \"" + toObj->GetName() +
          "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetArrayToObject(GmatBase *toObj, const std::string &fromArray)
//-------------------------------------------------------------------------------
bool Interpreter::SetArrayToObject(GmatBase *toObj, const std::string &fromArray)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToObject() toObj=%s, fromArray=%s\n",
       toObj->GetName().c_str(), fromArray.c_str());
   #endif
   
   if (toObj->GetTypeName() != "Variable")
   {
      InterpreterException ex
         ("Setting \"" + fromArray + "\" to an object \"" + toObj->GetName() +
          "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   Integer row, col;
   Parameter *param = GetArrayIndex(fromArray, row, col);
   if (param == NULL)
      return false;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   SetArrayToObject() row=%d, col=%d\n", row, col);
   #endif
   
   // Check for array index
   if (row == -1 || col == -1)
   {
      InterpreterException ex("Invalid array index: " + fromArray);
      HandleError(ex);
      return false;
   }
   
   Real rval = GetArrayValue(fromArray, row, col);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("   SetArrayToObject() rval=%f\n", rval);
   #endif

   try
   {
      toObj->SetRealParameter("Value", rval);
   }
   catch (BaseException &e)
   {
      HandleError(e);
      return false;
   }
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetValueToObject(GmatBase *toObj, const std::string &value)
//-------------------------------------------------------------------------------
bool Interpreter::SetValueToObject(GmatBase *toObj, const std::string &value)
{
   std::string objType = toObj->GetTypeName();
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetValueToObject() objType=<%s>, toObj=%s, value=<%s>\n",
       objType.c_str(), toObj->GetName().c_str(), value.c_str());
   #endif
   
   if (objType != "Variable" && objType != "String")
   {
      InterpreterException ex
         ("Setting \"" + value + "\" to an object \"" + toObj->GetName() +
          "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   if (objType == "String")
   {
      std::string valueToUse = GmatStringUtil::RemoveEnclosingString(value, "'");
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("   Calling %s->SetStringParameter(Value, %s)\n", toObj->GetName().c_str(),
          valueToUse.c_str());
      #endif
      
      toObj->SetStringParameter("Value", valueToUse);
   }
   else if (objType == "Variable")
   {
      Real rval;

      try
      {
         if (GmatStringUtil::ToReal(value, rval, true))
         {      
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage("   SetValueToObject() rval=%f\n", rval);
            #endif
         
            toObj->SetRealParameter("Value", rval);
         }
         else
         {
            InterpreterException ex
               ("Setting \"" + value + "\" to an object \"" + toObj->GetName() +
                "\" is not allowed");
            HandleError(ex);
            return false;
         }
      }
      catch (BaseException &e)
      {
         HandleError(e);
         return false;
      }
   }
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetObjectToProperty(GmatBase *toOwner, const std::string &toProp,
//                          GmatBase *fromObj)
//-------------------------------------------------------------------------------
bool Interpreter::SetObjectToProperty(GmatBase *toOwner, const std::string &toProp,
                                      GmatBase *fromObj)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToProperty() ownerType=%s, toOwner=%s, toProp=%s, "
       "fromObj=%s\n", toOwner->GetTypeName().c_str(), toOwner->GetName().c_str(),
       toProp.c_str(), fromObj->GetName().c_str());
   #endif
   
   if (toOwner->GetType() == Gmat::FORCE_MODEL)
   {
      std::string objName = fromObj->GetName();
      bool retval = SetForceModelProperty(toOwner, toProp, objName, fromObj);
      if (!retval)
      {
         InterpreterException ex
            ("The value of \"" + objName + "\" for field \"" + toProp +
             "\" on ForceModel \"" + toOwner->GetName() + "\" is not an allowed value");
         HandleError(ex);
         return false;
      }
      
      return true;
   }
   
   
   GmatBase *toObj = NULL;
   Integer toId = -1;
   Gmat::ParameterType toType;
   
   try
   {
      FindPropertyID(toOwner, toProp, &toObj, toId, toType);
      
      if (toObj == NULL)
      {
         if (parsingDelayedBlock)
         {
            InterpreterException ex
               ("The field name \"" + toProp + "\" on object " + toOwner->GetName() +
                " is not permitted");
            HandleErrorMessage(ex, lineNumber, currentLine, true);
            return false;
         }
         
         delayedBlocks.push_back(currentBlock);
         std::string lineNumStr = GmatStringUtil::ToString(theReadWriter->GetLineNumber());
         delayedBlockLineNumbers.push_back(lineNumStr);
            
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("   ===> added to delayed blocks: line:%s, %s\n", lineNumStr.c_str(),
             currentBlock.c_str());
         #endif
         
         return true;
      }
   }
   catch (BaseException &e)
   {
      if (parsingDelayedBlock)
         return false;
      
      delayedBlocks.push_back(currentBlock);
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("   ===> added to delayed blocks: %s\n", currentBlock.c_str());
      #endif
      
      return true;
   }
   
   toType = toObj->GetParameterType(toId);
   
   // Let's treat enumeration type as string type
   if (toType == Gmat::ENUMERATION_TYPE)
      toType = Gmat::STRING_TYPE;
   
   try
   {
      std::string fromTypeName = fromObj->GetTypeName();
      
      if (fromObj->GetType() == Gmat::PARAMETER)
      {
         Gmat::ParameterType fromType = ((Parameter*)fromObj)->GetReturnType();
         
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("   toId=%d, fromType=%d, toType=%d\n", toId, fromType, toType);
         #endif
         
         if (fromType == toType)
         {
            if (toType == Gmat::STRING_TYPE)
               toObj->SetStringParameter(toId, fromObj->GetStringParameter("Value"));
            else if (toType == Gmat::REAL_TYPE)
               toObj->SetRealParameter(toId, fromObj->GetRealParameter("Value"));
         }
         else
         {
            bool errorCond = false;
            if (fromTypeName == "String")
            {
               if (toType == Gmat::STRING_TYPE || toType == Gmat::STRINGARRAY_TYPE)
                  toObj->SetStringParameter(toId, fromObj->GetStringParameter("Value"));
               else if (toType == Gmat::OBJECT_TYPE || toType == Gmat::OBJECTARRAY_TYPE)
                  toObj->SetStringParameter(toId, fromObj->GetName());               
               else
                  errorCond = true;
            }
            else if (fromTypeName == "Variable")
            {
               if (toType == Gmat::REAL_TYPE)
                  toObj->SetRealParameter(toId, fromObj->GetRealParameter("Value"));
               else
                  errorCond = true;
            }
            else
            {
               if (toType == Gmat::OBJECT_TYPE || toType == Gmat::OBJECTARRAY_TYPE)
                  toObj->SetStringParameter(toId, fromObj->GetName());
               else
                  errorCond = true;
            }
            
            if (errorCond)
            {
               InterpreterException ex
                  ("The value of \"" + fromObj->GetName() + "\" for field \"" + toProp +
                   "\" on object " + "\"" + toOwner->GetName() + "\" is not an allowed value");
               HandleError(ex);
               return false;
            }
         }
      }
      else
      {
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("   Setting objType=%s, objName=%s\n", fromTypeName.c_str(),
             fromObj->GetName().c_str());
         #endif
         
         toObj->SetStringParameter(toProp, fromObj->GetName());
         if (toObj->IsOwnedObject(toId))
            toObj->SetRefObject(fromObj, fromObj->GetType(), fromObj->GetName());
      }
   }
   catch (BaseException &ex)
   {
      HandleError(ex);
      return false;
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToProperty() returning true\n");
   #endif
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetPropertyToProperty(GmatBase *toOwner, const std::string &toProp,
//                            GmatBase *fromOwner, const std::string &fromProp)
//-------------------------------------------------------------------------------
bool Interpreter::SetPropertyToProperty(GmatBase *toOwner, const std::string &toProp,
                                        GmatBase *fromOwner, const std::string &fromProp)
{
   bool retval = true;
   errorMsg1 = "";
   errorMsg2 = "";
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetPropertyToProperty() toOwner=%s<%s>, toProp=%s, fromOwner=%s, fromProp=%s\n",
       toOwner->GetName().c_str(), toOwner->GetTypeName().c_str(), toProp.c_str(),
       fromOwner->GetName().c_str(), fromProp.c_str());
   #endif
   
   Integer toId = -1;
   Gmat::ParameterType toType = Gmat::UNKNOWN_PARAMETER_TYPE;
   std::string lhs = toOwner->GetName() + "." + toProp;
   std::string rhs = fromOwner->GetName() + "." + fromProp;
   std::string value;
   Parameter *lhsParam = NULL;
   Parameter *rhsParam = NULL;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("   lhs=%s, rhs=%s\n", lhs.c_str(), rhs.c_str());
   #endif

   //-----------------------------------
   // try LHS property
   //-----------------------------------
   
   try
   {
      GmatBase *toObj = NULL;
      FindPropertyID(toOwner, toProp, &toObj, toId, toType);
   }
   catch (BaseException &e)
   {
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("   Parameter ID of '%s' not found. So create a parameter '%s'\n",
          toProp.c_str(), lhs.c_str());
      #endif
      lhsParam = CreateSystemParameter(lhs);
   }
   
   //-----------------------------------
   // try RHS property
   //-----------------------------------
   // try create parameter first if rhs type is OBJECT_TYPE
   if (toType == Gmat::OBJECT_TYPE)
      rhsParam = CreateSystemParameter(rhs);
   
   Integer fromId = -1;
   Gmat::ParameterType fromType = Gmat::UNKNOWN_PARAMETER_TYPE;
   bool isRhsProperty = true;
   
   try
   {
      fromId = fromOwner->GetParameterID(fromProp);   
      fromType = fromOwner->GetParameterType(fromId);
   }
   catch (BaseException &e)
   {
      isRhsProperty = false;
      fromType = Gmat::STRING_TYPE;
   }
   
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   toId=%d, toType=%d, fromId=%d, fromType=%d, lhsParam=%p, rhsParam=%p\n",
       toId, toType, fromId, fromType, lhsParam, rhsParam);
   #endif
   
   //-----------------------------------
   // now set value
   //-----------------------------------
   
   if (lhsParam != NULL && rhsParam != NULL)
   {
      SetObjectToObject(lhsParam, rhsParam);
   }
   else if (lhsParam == NULL && rhsParam != NULL)
   {
      if (toType == rhsParam->GetReturnType())
      {
         value = rhsParam->ToString();
         retval = SetProperty(toOwner, toId, toType, value);
      }
      else
      {
         retval = SetProperty(toOwner, toId, toType, rhs);
      }
   }
   else if (lhsParam != NULL && rhsParam == NULL)
   {
      if (lhsParam->GetReturnType() == fromType)
      {
         value = GetPropertyValue(fromOwner, fromId);
         lhsParam->SetString(value); 
         retval = true;
      }
   }
   else if (lhsParam == NULL && rhsParam == NULL)
   {
      if (toType == fromType)
      {
         if (toType == Gmat::STRING_TYPE || toType == Gmat::ENUMERATION_TYPE)
         {
            if (isRhsProperty)
            {
               value = GetPropertyValue(fromOwner, fromId);
               retval = SetPropertyValue(toOwner, toId, toType, value);
            }
            else
            {
               retval = SetPropertyValue(toOwner, toId, toType, rhs);
            }
         }
         else
         {
            value = GetPropertyValue(fromOwner, fromId);
            retval = SetProperty(toOwner, toId, toType, value);
         }
      }
      else
      {
         retval = SetProperty(toOwner, toId, toType, rhs);
      }
   }
   
   if (!retval)
   {
      if (errorMsg1 == "")
      {
         InterpreterException ex
            ("The field name \"" + fromProp + "\" on object " + toOwner->GetName() +
             " is not permitted");
         HandleError(ex);
      }
      else
      {
         InterpreterException ex
            (errorMsg1 + "field \"" + toProp + "\" on object " + "\"" +
             toOwner->GetName() + "\" is not an allowed value." + errorMsg2);
         HandleError(ex);
      }
   }
   
   return retval;
}


//-------------------------------------------------------------------------------
// bool SetArrayToProperty(GmatBase *toOwner, const std::string &toProp,
//                         const std::string &fromArray)
//-------------------------------------------------------------------------------
bool Interpreter::SetArrayToProperty(GmatBase *toOwner, const std::string &toProp,
                                     const std::string &fromArray)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToProperty() toOwner=%s, toProp=%s, fromArray=%s\n",
       toOwner->GetName().c_str(), toProp.c_str(), fromArray.c_str());
   #endif
   
   Integer toId = -1;
   Gmat::ParameterType toType = Gmat::UNKNOWN_PARAMETER_TYPE;
   
   // Check for property id
   try
   {
      toId = toOwner->GetParameterID(toProp);
      toType = toOwner->GetParameterType(toId);
   }
   catch (BaseException &ex)
   {
      HandleError(ex);
      return false;
   }
   
   // Property type must be Real type, so check
   if (toType != Gmat::REAL_TYPE)
   {
      InterpreterException ex
         ("The value of \"" + fromArray + "\" for field \"" + toProp +
          "\" on object " + "\"" + toOwner->GetName() + "\" is not an allowed value");
      HandleError(ex);
      return false;
   }
      
   // Now try to set array to property
   Integer row, col;
   Real rval = GetArrayValue(fromArray, row, col);
   
   try
   {
      toOwner->SetRealParameter(toId, rval);
   }
   catch (BaseException &e)
   {
      HandleError(e);
      return false;
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToProperty() exiting. rval=%f, row=%d, col=%d, \n",
       rval, row, col);
   #endif
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetValueToProperty(GmatBase *toOwner, const std::string &toProp,
//                         const std::string &value)
//-------------------------------------------------------------------------------
bool Interpreter::SetValueToProperty(GmatBase *toOwner, const std::string &toProp,
                                     const std::string &value)
{
   bool retval = false;
   errorMsg1 = "";
   errorMsg2 = "";
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetValueToProperty() objType=%s, objName=%s, toProp=%s, "
       "value=%s\n", toOwner->GetTypeName().c_str(), toOwner->GetName().c_str(),
       toProp.c_str(), value.c_str());
   #endif
   
   if (toOwner->GetType() == Gmat::FORCE_MODEL)
   {
      retval = SetForceModelProperty(toOwner, toProp, value, NULL);
   }
   else if (toOwner->GetType() == Gmat::SOLAR_SYSTEM)
   {
      retval = SetSolarSystemProperty(toOwner, toProp, value);
   }
   else
   {
      StringArray parts = theTextParser.SeparateDots(toProp);
      
      // if property has multiple dots, handle separately
      if (parts.size() > 1)
      {
         retval = SetComplexProperty(toOwner, toProp, value);
      }
      else
      {
         GmatBase *toObj = NULL;
         Integer toId = -1;
         Gmat::ParameterType toType;
         
         FindPropertyID(toOwner, toProp, &toObj, toId, toType);
         
         if (toObj == NULL)
         {
            if (parsingDelayedBlock)
            {
               InterpreterException ex
                  ("The field name \"" + toProp + "\" on object " + toOwner->GetName() +
                   " is not permitted");
               HandleErrorMessage(ex, lineNumber, currentLine, true);
               return false;
            }
            
            delayedBlocks.push_back(currentBlock);
            std::string lineNumStr = GmatStringUtil::ToString(theReadWriter->GetLineNumber());
            delayedBlockLineNumbers.push_back(lineNumStr);
            
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   ===> added to delayed blocks: line:%s, %s\n", lineNumStr.c_str(),
                currentBlock.c_str());
            #endif
            
            return true;
         }
         
         retval = SetProperty(toObj, toId, toType, value);
      }
   }
   
   if (retval == false && !ignoreError)
   {
      if (errorMsg1 == "")
      {
         InterpreterException ex
            ("The field name \"" + toProp + "\" on object " + "\"" +
             toOwner->GetName() + "\" is not permitted");
         HandleError(ex);
      }
      else
      {
         InterpreterException ex
            (errorMsg1 + "field \"" + toProp + "\" on object " + "\"" +
             toOwner->GetName() + "\" is not an allowed value." + errorMsg2);
         HandleError(ex);
      }
   }
   
   if (ignoreError)
      ignoreError = false;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetValueToProperty() returning retval=%d\n", retval);
   #endif
   
   return retval;
}


//-------------------------------------------------------------------------------
// bool SetObjectToArray(GmatBase *toArrObj, const std::string &toArray,
//                       GmatBase *fromObj)
//-------------------------------------------------------------------------------
bool Interpreter::SetObjectToArray(GmatBase *toArrObj, const std::string &toArray,
                                   GmatBase *fromObj)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToArray() toArrObj=%s, toArray=%s, fromObj=%s\n",
       toArrObj->GetName().c_str(), toArray.c_str(), fromObj->GetName().c_str());
   #endif
   
   if (fromObj->GetTypeName() != "Variable")
   {
      //InterpreterException ex
      //   ("Cannot set object other than Variable or Array element.");
      InterpreterException ex
         ("Setting object \"" + fromObj->GetName() + "\" to an array \"" + toArray +
          "\" is not permitted.");
      HandleError(ex);
      return false;
   }
   
   Real rval = fromObj->GetRealParameter("Value");
   
   Integer row, col;
   Parameter *param = GetArrayIndex(toArray, row, col);
   if (param == NULL)
      return false;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   SetObjectToArray()rval=%f, row=%d, col=%d\n", rval, row, col);
   #endif

   try
   {
      toArrObj->SetRealParameter("SingleValue", rval, row, col);
   }
   catch (BaseException &e)
   {
      HandleError(e);
      return false;
   }

   return true;
}


//-------------------------------------------------------------------------------
// bool SetPropertyToArray(GmatBase *toArrObj, const std::string &toArray,
//                         GmatBase *fromOwner, const std::string &fromProp)
//-------------------------------------------------------------------------------
bool Interpreter::SetPropertyToArray(GmatBase *toArrObj, const std::string &toArray,
                                     GmatBase *fromOwner, const std::string &fromProp)
{
   #ifdef DEBGU_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyToArray() toArrObj=%s, toArray=%s, fromOwner=%s, "
       "fromProp=%s\n", toArrObj->GetName().c_str(), toArray.c_str(),
       fromOwner->GetName().c_str(), fromProp.c_str());
   #endif
   
   // get object parameter id
   Integer fromId = fromOwner->GetParameterID(fromProp);
   
   if (fromOwner->GetParameterType(fromId) != Gmat::REAL_TYPE)
   {
      InterpreterException ex
         ("Setting non-Real type of \"" + fromProp + "\" to an Array element \"" +
          toArray + "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   Real rval = fromOwner->GetRealParameter(fromId);
   
   Integer row, col;
   Parameter *param = GetArrayIndex(toArray, row, col);
   if (param == NULL)
      return false;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   SetPropertyToArray()rval=%f, row=%d, col=%d\n", rval, row, col);
   #endif

   try
   {
      toArrObj->SetRealParameter("SingleValue", rval, row, col);
   }
   catch (BaseException &e)
   {
      HandleError(e);
      return false;
   }

   return true;
}


//-------------------------------------------------------------------------------
// bool SetArrayToArray(GmatBase *toArrObj, const std::string &toArray,
//                      GmatBase *fromArrObj, const std::string &fromArray)
//-------------------------------------------------------------------------------
bool Interpreter::SetArrayToArray(GmatBase *toArrObj, const std::string &toArray,
                                  GmatBase *fromArrObj, const std::string &fromArray)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToArray() toArrObj=%s, toArray=%s, "
       "fromArrObj=%s, fromArray=%s\n", toArrObj->GetName().c_str(),
       toArray.c_str(), fromArrObj->GetName().c_str(), fromArray.c_str());
   #endif
   
   Integer rowFrom, colFrom;
   Integer rowTo, colTo;
   
   Parameter *param = GetArrayIndex(toArray, rowTo, colTo);
   if (param == NULL)
      return false;
   
   param = GetArrayIndex(fromArray, rowFrom, colFrom);
   if (param == NULL)
      return false;
   
   Real rval = GetArrayValue(fromArray, rowFrom, colFrom);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   SetArrayToArray() rval=%f, rowFrom=%d, colFrom=%d, \n",
       rval, rowFrom, colFrom);
   MessageInterface::ShowMessage
      ("   SetArrayToArray()rowTo=%d, colTo=%d\n", rowTo, colTo);
   #endif

   try
   {
      if (fromArray[0] == '-')
         toArrObj->SetRealParameter("SingleValue", rval, rowTo, colTo);
      else   
         toArrObj->SetRealParameter("SingleValue", -rval, rowTo, colTo);
   }
   catch (BaseException &e)
   {
      HandleError(e);
      return false;
   }

   return true;
}


//-------------------------------------------------------------------------------
// bool SetValueToArray(GmatBase *array, const std::string &toArray,
//                      const std::string &value)
//-------------------------------------------------------------------------------
bool Interpreter::SetValueToArray(GmatBase *array, const std::string &toArray,
                                  const std::string &value)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetValueToArray() array=%s, toArray=%s, value=%s\n",
       array->GetName().c_str(), toArray.c_str(), value.c_str());
   #endif
   
   Integer row, col;
   Real rval;

   Parameter *param = GetArrayIndex(toArray, row, col);
   if (param == NULL)
      return false;
   
   if (GmatStringUtil::ToReal(value, rval, true))
   {
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("   SetValueToArray() rval=%f, row=%d, col=%d\n", rval, row, col);
      #endif

      try
      {
         array->SetRealParameter("SingleValue", rval, row, col);
      }
      catch (BaseException &e)
      {
         InterpreterException ex("Index exceeds matrix dimensions");
         HandleError(ex);
         return false;
      }
   }
   else
   {
      InterpreterException ex
         ("Setting \"" + value + "\" to an object \"" + toArray +
          "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetPropertyValue(GmatBase *obj, const Integer id,
//                       const Gmat::ParameterType type,
//                       const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets parameters on GMAT objects.
 * 
 * @param  obj    Pointer to the object that owns the property.
 * @param  id     ID for the property.
 * @param  type   Type for the property.
 * @param  value  Value of the property.
 * @param  index  Index of the property in array.
 * 
 * @return true if the property is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Interpreter::SetPropertyValue(GmatBase *obj, const Integer id,
                                   const Gmat::ParameterType type,
                                   const std::string &value, const Integer index)
{
   bool retval = false;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyValue() obj=%s, id=%d, type=%d, value=%s, index=%d\n",
       obj->GetName().c_str(), id, type, value.c_str(), index);
   #endif
   
   std::string valueToUse = value;
   CheckForSpecialCase(obj, id, valueToUse);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   propertyType=%s\n", GmatBase::PARAM_TYPE_STRING[type].c_str());
   #endif
   
   
   switch (type)
   {
   case Gmat::OBJECT_TYPE:
   case Gmat::OBJECTARRAY_TYPE:
      {
         return SetPropertyObjectValue(obj, id, type, valueToUse, index);
      }
   case Gmat::ENUMERATION_TYPE:
   case Gmat::STRING_TYPE:
   case Gmat::STRINGARRAY_TYPE:
      {
         return SetPropertyStringValue(obj, id, type, valueToUse, index);
      }
   case Gmat::INTEGER_TYPE:
   case Gmat::UNSIGNED_INT_TYPE:
      {
         Integer ival;
         if (GmatStringUtil::ToInteger(valueToUse, ival))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetIntegerParameter(%d, %d)\n",
                obj->GetName().c_str(), id, ival);
            #endif
            
            obj->SetIntegerParameter(id, ival);
            retval = true;
         }
         else
         {
            errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" for ";
            errorMsg2 = " Only integer number is allowed";
         }
         break;
      }
   case Gmat::UNSIGNED_INTARRAY_TYPE:
      {
         Integer ival;
         if (GmatStringUtil::ToInteger(valueToUse, ival))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetUnsignedIntParameter(%d, %d, %d)\n",
                obj->GetName().c_str(), id, ival, index);
            #endif
            
            obj->SetUnsignedIntParameter(id, ival, index);
            retval = true;
         }
         else
         {
            errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" for ";
            errorMsg2 = " Only integer number is allowed";
         }
         break;
      }
   case Gmat::REAL_TYPE:
   case Gmat::RVECTOR_TYPE:
      {
         Real rval;
         if (GmatStringUtil::ToReal(valueToUse, rval, true))
         {
            #ifdef DEBUG_SET
            std::string rvalStr =
               GmatStringUtil::ToString(rval, false, false, 17, 16);
            MessageInterface::ShowMessage
               ("   Calling %s->SetRealParameter(%d, %s)\n",
                obj->GetName().c_str(), id, rvalStr.c_str());
            #endif
            
            if (type == Gmat::REAL_TYPE)
               obj->SetRealParameter(id, rval);
            else
               obj->SetRealParameter(id, rval, index);
               
            retval = true;
         }
         else
         {
            errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" for ";
            errorMsg2 = " The allowed value is Real number";
         }
         break;
      }
   case Gmat::BOOLEAN_TYPE:
      {
         bool tf;
         if (GmatStringUtil::ToBoolean(value, tf))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetBooleanParameter(%d, %d)\n",
                obj->GetName().c_str(), id, tf);
            #endif
            
            obj->SetBooleanParameter(id, tf);
            retval = true;
         }
         else
         {
            errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" for ";
            errorMsg2 = " The allowed values are: [true false]";
         }
         break;
      }
   case Gmat::ON_OFF_TYPE:
      {
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("   Calling %s->SetOnOffParameter(%d, %s)\n",
             obj->GetName().c_str(), id, valueToUse.c_str());
         #endif
         
         if (valueToUse == "On" || valueToUse == "Off")
         {
            retval = obj->SetOnOffParameter(id, valueToUse);
         }
         else
         {
            errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" for ";
            errorMsg2 = " The allowed values are: [On Off]";
         }
         break;
      }
   default:
      InterpreterException ex
         ("Interpreter::SetPropertyValue() Cannot handle the type: " +
          GmatBase::PARAM_TYPE_STRING[type] + " yet.\n");
      HandleError(ex);
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyValue() returning retval=%d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool SetPropertyObjectValue(GmatBase *obj, const Integer id, ...)
//------------------------------------------------------------------------------
bool Interpreter::SetPropertyObjectValue(GmatBase *obj, const Integer id,
                                         const Gmat::ParameterType type,
                                         const std::string &value,
                                         const Integer index)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyObjectValue() obj=%s, id=%d, type=%d, value=%s, "
       "index=%d\n", obj->GetName().c_str(), id, type, value.c_str(), index);
   #endif
   
   Parameter *param = NULL;
   
   // Try creating Parameter first if it is not ObjectType
   if (!IsObjectType(value))
   {
      // It is not a one of object types, so create parameter
      param = CreateSystemParameter(value);
      
      #ifdef DEBUG_SET
      if (param)
         MessageInterface::ShowMessage
            ("   param=(%p)%s type=%s returnType=%d\n", param,
             param->GetName().c_str(), param->GetTypeName().c_str(),
             param->GetReturnType());
      #endif
   }
   else
   {
      // It is object type so get parameter (Bug 743 fix)
      param = theModerator->GetParameter(value);
   }
   
   try
   {
      if (param != NULL)
      {
         // Other than Subscriber, it can only take STRING_TYPE parameter
         if (param->GetReturnType() == Gmat::STRING_TYPE ||
             obj->IsOfType(Gmat::SUBSCRIBER))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetStringParameter(%d, %s)\n",
                obj->GetName().c_str(), id, value.c_str());
            #endif
            
            // Let base code check for the invalid values
            obj->SetStringParameter(id, value);
         }
         else
         {
            errorMsg1 = errorMsg1 + "The value of \"" + value + "\" for ";
            errorMsg2 = "  The allowed value is Object Name";
            return false;
         }
      }
      else
      {
         // check if value is a number
         Real rval;
         Integer ival;
         if (GmatStringUtil::ToReal(value, rval, true) ||
             GmatStringUtil::ToInteger(value, ival, true))
         {
            // Handle special case for OpenGlPlot. (loj: 2008.04.01)
            // ViewPointReference, ViewPointVector, and ViewDirection can have 
            // both vector and object name.
            if (obj->GetTypeName() == "OpenGLPlot")
            {
               obj->SetStringParameter(id, value, index);
            }
            else
            {
               errorMsg1 = errorMsg1 + "The value of \"" + value + "\" for ";
               errorMsg2 = "  The allowed value is Object Name";
               return false;
            }
         }
         
         // check if value is an object name
         GmatBase *configObj = FindObject(value);
         if (configObj)
         {
            // Set as String parameter, so it can be validated in FinalPass()
            bool retval = true;
            if (index != -1)
            {
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling %s->SetStringParameter(%d, %s, %d)\n",
                   obj->GetName().c_str(), id, value.c_str(), index);
               #endif
               
               retval = obj->SetStringParameter(id, value, index);
            }
            
            // if it has no index or failed setting with index, try without index
            if (index == -1 || !retval)
            {
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling %s->SetStringParameter(%d, %s)\n",
                   obj->GetName().c_str(), id, value.c_str());
               #endif
               
               obj->SetStringParameter(id, value);
            }
         }
         else
         {
            // Create Owned Object, if it is valid owned object type
            GmatBase *ownedObj = NULL;
            if (obj->IsOwnedObject(id))
               ownedObj = CreateObject(value, "");
            
            #ifdef DEBUG_SET
            if (ownedObj)
               MessageInterface::ShowMessage
                  ("   Created ownedObjType: %s\n", ownedObj->GetTypeName().c_str());
            #endif
            
            if (ownedObj)
            {
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling %s->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
                   ownedObj->GetTypeName().c_str(), ownedObj, ownedObj->GetType());
               #endif
               
               obj->SetRefObject(ownedObj, ownedObj->GetType(), ownedObj->GetName());
            }
            else
            {
               // Special case of InternalForceModel in script
               if (value != "InternalForceModel")
               {
                  // Set as String parameter, so it can be caught in FinalPass()
                  #ifdef DEBUG_SET
                  MessageInterface::ShowMessage
                     ("   Calling %s->SetStringParameter(%d, %s)\n",
                      obj->GetName().c_str(), id, value.c_str());
                  #endif
                  
                  obj->SetStringParameter(id, value);
               }
            }
         }
      }
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("Interpreter::SetPropertyObjectValue() returning true\n");
      #endif
      
      return true;
   }
   catch (BaseException &ex)
   {
      HandleError(ex);
      ignoreError = true;
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("Interpreter::SetPropertyObjectValue() returning false\n");
      #endif
      
      return false;
   }
}


//------------------------------------------------------------------------------
// bool SetPropertyStringValue(GmatBase *obj, const Integer id, ...)
//------------------------------------------------------------------------------
bool Interpreter::SetPropertyStringValue(GmatBase *obj, const Integer id,
                                         const Gmat::ParameterType type,
                                         const std::string &value,
                                         const Integer index)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyStringValue() obj=%s, id=%d, type=%d, value=%s, "
       "index=%d\n", obj->GetName().c_str(), id, type, value.c_str(), index);
   #endif
   
   bool retval = true;
   std::string valueToUse = value;
   
   switch (type)
   {
   case Gmat::ENUMERATION_TYPE:
   case Gmat::STRING_TYPE:
      {
         // remove enclosing quotes if used
         valueToUse = GmatStringUtil::RemoveEnclosingString(valueToUse, "'");
         
         try
         {
            if (index >= 0)
            {
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling %s->SetStringParameter(%d, %s, %d)\n",
                   obj->GetName().c_str(), id, valueToUse.c_str(), index);
               #endif
               
               retval = obj->SetStringParameter(id, valueToUse, index);
            }
            else
            {
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling %s->SetStringParameter(%d, %s)\n",
                   obj->GetName().c_str(), id, valueToUse.c_str());
               #endif
               
               retval = obj->SetStringParameter(id, valueToUse);
            }
         }
         catch (BaseException &ex)
         {
            HandleError(ex);
            ignoreError = true;
            retval = false;
         }
         break;
      }
   case Gmat::STRINGARRAY_TYPE:
      {         
         try
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetStringParameter(%d, %s)\n",
                obj->GetName().c_str(), id, valueToUse.c_str());
            #endif
            
            retval = obj->SetStringParameter(id, valueToUse);
         }
         catch (BaseException &e)
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetStringParameter(%d, %s, %d)\n", id,
                valueToUse.c_str(), index);
            #endif
            
            // try with index
            retval = obj->SetStringParameter(id, valueToUse, index);
         }
         break;
      }
   default:
      break;
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyStringValue() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// std::string GetPropertyValue(GmatBase *obj, const Integer id)
//------------------------------------------------------------------------------
std::string Interpreter::GetPropertyValue(GmatBase *obj, const Integer id)
{
   std::string sval;
   
   Gmat::ParameterType type = obj->GetParameterType(id);
   
   if (type == Gmat::OBJECT_TYPE)
   {
      sval = obj->GetStringParameter(id);
   }
   else if (type == Gmat::INTEGER_TYPE)
   {
      sval = GmatStringUtil::ToString(obj->GetIntegerParameter(id));
   }
   else if (type == Gmat::UNSIGNED_INT_TYPE)
   {
      sval = GmatStringUtil::ToString(obj->GetIntegerParameter(id));
   }
//    else if (type == Gmat::UNSIGNED_INTARRAY_TYPE)
//    {
//       sval = GmatStringUtil::ToString(obj->GetIntegerParameter(id));
//    }
   else if (type == Gmat::REAL_TYPE)
   {
      sval = GmatStringUtil::ToString(obj->GetRealParameter(id));
   }
   else if (type == Gmat::STRING_TYPE || type == Gmat::ENUMERATION_TYPE)
   {
      sval = obj->GetStringParameter(id);
   }
//    else if (type == Gmat::STRINGARRAY_TYPE)
//    {
//       sval = obj->GetStringParameter(id));
//    }
   else if (type == Gmat::BOOLEAN_TYPE)
   {
      if (obj->GetBooleanParameter(id))
         sval = "true";
      else
         sval = "false";
   }
   else if (type == Gmat::ON_OFF_TYPE)
   {
      sval = obj->GetOnOffParameter(id);
   }
   
   return sval;
}


//------------------------------------------------------------------------------
// bool SetProperty(GmatBase *obj, const Integer id, const Gmat::ParameterType type
//                  const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets parameters on GMAT objects.
 * 
 * @param  obj    Pointer to the object that owns the property.
 * @param  id     property ID
 * @param  type   proerty Type
 * @param  value  Value of the property.
 * 
 * @return true if the property is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Interpreter::SetProperty(GmatBase *obj, const Integer id,
                              const Gmat::ParameterType type,
                              const std::string &value)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetProperty() obj=%s, id=%d, type=%d, value=%s\n",
       obj->GetName().c_str(), id, type, value.c_str());
   #endif
   
   bool retval = false;
   
   std::string valueToUse = value;
   CheckForSpecialCase(obj, id, valueToUse);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("   propertyType=%d\n", obj->GetParameterType(id));
   #endif
   
   StringArray rhsValues;
   Integer count = 0;
   
   // if value has braces, setting multiple values
   if (value.find("{") != value.npos || value.find("}") != value.npos)
      rhsValues = theTextParser.SeparateBrackets(value, "{}", " ,");
   else if (value.find("[") != value.npos || value.find("]") != value.npos)
      rhsValues = theTextParser.SeparateBrackets(value, "[]", " ,");
   
   count = rhsValues.size();
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("   count=%d\n", count);
   #endif
   
   if (count > 0)
   {
      for (int i=0; i<count; i++)
         retval = SetPropertyValue(obj, id, type, rhsValues[i], i);
   }
   else
   {
      retval = SetPropertyValue(obj, id, type, value);
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetProperty() returning retval=%d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool SetComplexProperty(GmatBase *obj, const std::string &prop,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool Interpreter::SetComplexProperty(GmatBase *obj, const std::string &prop,
                                     const std::string &value)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetComplexProperty() prop=%s, value=%s\n",
       prop.c_str(), value.c_str());
   #endif
   
   StringArray parts = theTextParser.SeparateDots(prop);

   if (obj->GetType() == Gmat::SPACECRAFT)
   {
      Spacecraft *sc = (Spacecraft*)obj;
      
      if (parts[0] == "Epoch")
      {
         sc->SetDateFormat(parts[1]);
         sc->SetEpoch(value);
      }
      else
      {
         return false;
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetForceModelProperty(GmatBase *obj, const std::string &prop,
//                            const std::string &value, GmatBase *fromObj)
//------------------------------------------------------------------------------
bool Interpreter::SetForceModelProperty(GmatBase *obj, const std::string &prop,
                                        const std::string &value, GmatBase *fromObj)
{
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(prop);
   Integer count = parts.size();
   std::string pmType = parts[count-1];
   Integer id;
   Gmat::ParameterType type;
   
   // Current ForceModel scripting, SRP is on for central body.
   //GMAT FM.CentralBody = Earth;
   //GMAT FM.PrimaryBodies = {Earth, Luna};
   //GMAT FM.PointMasses = {Sun, Jupiter}; 
   //GMAT FM.Drag = None;
   //GMAT FM.SRP = On;
   //GMAT FM.GravityField.Earth.Degree = 20;
   //GMAT FM.GravityField.Earth.Order = 20;
   //GMAT FM.GravityField.Earth.Model = JGM2.cof;
   //GMAT FM.GravityField.Luna.Degree = 4;
   //GMAT FM.GravityField.Luna.Order = 4;
   //GMAT FM.GravityField.Luna.PotentialFile = LP165P.cof;
   
   // For future scripting we want to specify body for Drag and SRP
   // e.g. FM.Drag.Earth = JacchiaRoberts;
   //      FM.Drag.Mars = MarsAtmos;
   //      FM.SRP.ShadowBodies = {Earth,Moon}
   
   ForceModel *forceModel = (ForceModel*)obj;
   std::string forceType = ForceModel::GetScriptAlias(pmType);
   std::string centralBodyName = forceModel->GetStringParameter("CentralBody");
   
   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("Interpreter::SetForceModelProperty() fm=%s, prop=%s, value=%s\n"
       "   pmType=%s, forceType=%s\n", obj->GetName().c_str(), prop.c_str(), value.c_str(),
       pmType.c_str(), forceType.c_str());
   #endif
   
   //------------------------------------------------------------
   // Set ForceModel CentralBody
   //------------------------------------------------------------
   if (pmType == "CentralBody")
   {
      id = obj->GetParameterID("CentralBody");
      type = obj->GetParameterType(id);
      retval = SetPropertyValue(obj, id, type, value);
      return retval;
   }
   
   
   //------------------------------------------------------------
   // Create ForceModel owned PhysicalModel
   //------------------------------------------------------------
   
   if (pmType == "PrimaryBodies" || pmType == "PointMasses")
   {
      StringArray bodies = theTextParser.SeparateBrackets(value, "{}", " ,");
      
      for (UnsignedInt i=0; i<bodies.size(); i++)
      {
         #ifdef DEBUG_SET_FORCE_MODEL
         MessageInterface::ShowMessage("   bodies[%d]=%s\n", i, bodies[i].c_str());
         #endif
         
         // We don't want to configure PhysicalModel, so set name after create
         PhysicalModel *pm = (PhysicalModel*)CreateObject(forceType, "");
         if (pm)
         {
            pm->SetName(bodies[i]);
            
            if (!pm->SetStringParameter("BodyName", bodies[i]))
            {
               InterpreterException ex("Unable to set body for force " + bodies[i]);
               HandleError(ex);
            }
            
            #ifdef DEBUG_SET_FORCE_MODEL
            MessageInterface::ShowMessage
               ("   Adding type:<%s> name:<%s> to ForceModel:<%s>\n",
                pm->GetTypeName().c_str(), pm->GetName().c_str(),
                forceModel->GetName().c_str());
            #endif
            
            // Add force to ForceModel
            forceModel->AddForce(pm);
            
            // Use JGM2 for default Earth gravity file, in case it is not
            // specified in the script
            if (pmType == "PrimaryBodies" && bodies[i] == "Earth")
            {
               id = pm->GetParameterID("Model");
               type = pm->GetParameterType(id);
               retval = SetPropertyValue(pm, id, type, "JGM2");
            }
         }
      }
      
      return true;
   }
   else if (pmType == "SRP" || pmType == "Drag")
   {
      if (pmType == "SRP")
      {
         id = obj->GetParameterID("SRP");
         type = obj->GetParameterType(id);
         retval = SetPropertyValue(obj, id, type, value);
         
         if (retval && value != "On")
            return true;
         else if (!retval)
            return false;
      }
      
      if (pmType == "Drag" && value == "None")
         return true;
      
      // Create PhysicalModel
      PhysicalModel *pm = (PhysicalModel*)CreateObject(forceType, "");
      pm->SetName(centralBodyName);
      
      // Special handling for Drag
      if (pmType == "Drag")
      {
         if (!pm->SetStringParameter("AtmosphereModel", value))
         {
            InterpreterException ex
               ("Unable to set AtmosphereModel for drag force");
            HandleError(ex);
            ignoreError = true;
            return false;
         }
         
         /// @todo Add the body name for drag at other bodies
         if (value != "BodyDefault")
         {
            pm->SetStringParameter("BodyName", centralBodyName);
            GmatBase *am = CreateObject(value, "");
            if (am)
               pm->SetRefObject(am, Gmat::ATMOSPHERE, am->GetName());
            else
            {
               InterpreterException ex
                  ("Unable to create AtmosphereModel \"" + value + "\" for drag force");
               HandleError(ex);
               ignoreError = true;
               return false;
            }
         }
      }
      else if (pmType == "SRP")
      {
         // Should we set SRP on ForceModel central body?
         pm->SetStringParameter("BodyName", centralBodyName);
      }
      
      #ifdef DEBUG_SET_FORCE_MODEL
      MessageInterface::ShowMessage
         ("   Adding type:<%s> name:<%s> to ForceModel:<%s>\n",
          pm->GetTypeName().c_str(), pm->GetName().c_str(),
          forceModel->GetName().c_str());
      #endif
      
      // Add force to ForceModel
      forceModel->AddForce(pm);
      
      return true;
   }
   
   //------------------------------------------------------------
   // Set ForceModel owned object properties
   //------------------------------------------------------------
   
   pmType = parts[0];
   forceType = ForceModel::GetScriptAlias(pmType);
   std::string propName = parts[count-1];
   
   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("   Setting pmType=%s, forceType=%s, propName=%s\n", pmType.c_str(),
       forceType.c_str(), propName.c_str());
   #endif
   
   GmatBase *owner;
   Integer propId;
   Gmat::ParameterType propType;
   if (FindPropertyID(forceModel, propName, &owner, propId, propType))
   {
      id = owner->GetParameterID(propName);
      type = owner->GetParameterType(id);
      retval = SetPropertyValue(owner, id, type, value);
      if (fromObj != NULL)
         owner->SetRefObject(fromObj, fromObj->GetType(), value);
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool SetSolarSystemProperty(GmatBase *obj, const std::string &prop,
//                            const std::string &value)
//------------------------------------------------------------------------------
bool Interpreter::SetSolarSystemProperty(GmatBase *obj, const std::string &prop,
                                         const std::string &value)
{
   #ifdef DEBUG_SET_SOLAR_SYS
   MessageInterface::ShowMessage
      ("Interpreter::SetSolarSystemProperty() type=%s, name=%s, prop=%s, value=%s\n",
       obj->GetTypeName().c_str(), obj->GetName().c_str(), prop.c_str(), value.c_str());
   #endif
   
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(prop);
   Integer count = parts.size();
   SolarSystem *solarSystem = (SolarSystem *)obj;
   
   if (count == 1)
   {
      if (prop == "Ephemeris")
      {
         StringArray ephems = theTextParser.SeparateBrackets(value, "{}", " ,");
      
         #ifdef DEBUG_SET_SOLAR_SYS
         for (StringArray::iterator i = ephems.begin(); i != ephems.end(); ++i)
            MessageInterface::ShowMessage("   Source = %s\n", i->c_str());
         #endif
         
         theModerator->SetPlanetarySourceTypesInUse(ephems);
         retval = true;
      }
      else
      {
         Integer id = obj->GetParameterID(prop);
         Gmat::ParameterType type = obj->GetParameterType(id);
         retval = SetPropertyValue(obj, id, type, value);
      }
   }
   else
   {
      // Script has the form of:
      // GMAT SolarSystem.Earth.NutationUpdateInterval = 60.0;
      // GMAT SolarSystem.Earth.UseTTForEphemeris = true;
      // GMAT SolarSystem.Earth.DateFormat  = TAIModJulian;
      // GMAT SolarSystem.Earth.StateType   = Keplerian;
      // GMAT SolarSystem.Earth.InitalEpoch = 21544.500371
      // GMAT SolarSystem.Earth.SMA         = 149653978.978377
      
      std::string bodyName = parts[0];
      std::string newProp = parts[count-1];
      
      #ifdef DEBUG_SET_SOLAR_SYS
      MessageInterface::ShowMessage
         ("   bodyName=%s, newProp=%s\n", bodyName.c_str(), newProp.c_str());
      #endif
      
      // Cannot use FindPropertyID() because SolarSystem bodies have the
      // same property name. So use GetBody() instead.
      GmatBase *body = (GmatBase*)solarSystem->GetBody(bodyName);
      
      if (body == NULL)
      {
         InterpreterException ex
            ("Body: " + bodyName + " not found in the SolarSystem\n");
         HandleError(ex);
      }
      
      try
      {
         Integer id = body->GetParameterID(newProp);
         Gmat::ParameterType type = body->GetParameterType(id);
         retval = SetPropertyValue(body, id, type, value);
      }
      catch (BaseException &e)
      {
         HandleError(e);
      }
   }
   
   #ifdef DEBUG_SET_SOLAR_SYS
   MessageInterface::ShowMessage
      ("Interpreter::SetSolarSystemProperty() prop=%s, retval=%d\n",
       prop.c_str(), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool FindOwnedObject(GmatBase *owner, const std::string toProp,
//                      GmatBase **ownedObj, Integer &id, Gmat::OBJECT_TYPE &type)
//------------------------------------------------------------------------------
/*
 * Finds owned object and its property.
 *
 * @param  owner    Owner object to find owned object for property
 * @param  toProp   Property name to find
 * @param  id       Output owned property id (-1 if property not found)
 * @param  type     Output owned property type
 *                  (Gmat::UNKNOWN_PARAMETER_TYPE if property not found)
 *
 * @return  true if property found from the owned object
 */
//------------------------------------------------------------------------------
bool Interpreter::FindOwnedObject(GmatBase *owner, const std::string toProp,
                                  GmatBase **ownedObj, Integer &id,
                                  Gmat::ParameterType &type)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::FindOwnedObject() owner=%s, toProp=%s\n",
       owner->GetName().c_str(), toProp.c_str());
   #endif
   
   bool retval = false;
   Integer ownedObjCount = owner->GetOwnedObjectCount();
   Integer errorCount = 0;
   GmatBase *tempObj = NULL;
   
   // Initialize output parameters
   id = -1;
   type = Gmat::UNKNOWN_PARAMETER_TYPE;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("   ownedObjCount=%d\n", ownedObjCount);
   #endif
   
   if (ownedObjCount > 0)
   {
      for (int i=0; i<ownedObjCount; i++)
      {
         tempObj = owner->GetOwnedObject(i);
         if (ownedObj)
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   i=%d, ownedObj type=<%s>, name=<%s>\n", i,
                tempObj->GetTypeName().c_str(), tempObj->GetName().c_str());
            #endif
            
            try
            {
               id = tempObj->GetParameterID(toProp);
               type = tempObj->GetParameterType(id);
               *ownedObj = tempObj;
               retval = true;
               break;
            }
            catch (BaseException &e)
            {
               errorCount++;
               continue;
            }
         }
      }
      
      if (errorCount == ownedObjCount)
      {
         // Throw error only when parsing delayed block, so that
         // duplicated error message will not be shown
         if (parsingDelayedBlock)
         {
            //@todo
            // Currently SolarSystem parameter is handled by the Moderator,
            // so it is an exceptional case.
            // Eventually we want to move parameter handling to SolarSyatem.
            if (owner->GetName() != "SolarSystem")
            {
               InterpreterException ex
                  ("The field name \"" + toProp + "\" on object " + owner->GetName() +
                   " is not permitted");
               HandleErrorMessage(ex, lineNumber, currentLine, true);
            }
         }
      }
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   FindOwnedObject() returning retval=%d, ownedObj=%p\n",
       retval, *ownedObj);
   #endif
   
   return retval;
   
}


//------------------------------------------------------------------------------
// Real GetArrayValue(const std::string &arrayStr, Integer &row, Integer &col)
//------------------------------------------------------------------------------
/**
 * Retrives configured array value by row and col.
 *
 * @param  arrayStr  String form of array (A(1,3), B(2,j), etc)
 *
 * @note Array name must be created and configured before access.
 */
//------------------------------------------------------------------------------
Real Interpreter::GetArrayValue(const std::string &arrayStr,
                                Integer &row, Integer &col)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::GetArrayValue arrayStr=%s\n", arrayStr.c_str());
   #endif
   
   Parameter *param = GetArrayIndex(arrayStr, row, col);
   
   if (row != -1 && col != -1)
      return param->GetRealParameter("SingleValue", row, col);
   else
   {
      InterpreterException ex("Invalid row and column index\n");
      HandleError(ex);
   }
   return 0.0;
}


//------------------------------------------------------------------------------
// bool IsArrayElement(const std::string &str)
//------------------------------------------------------------------------------
bool Interpreter::IsArrayElement(const std::string &str)
{
   bool retval = false;
   
   if (str.find("[") != str.npos)
   {
      InterpreterException ex("\"" + str + "\" is not a valid Array element");
      HandleError(ex);
   }
   
   retval = GmatStringUtil::IsParenPartOfArray(str);

   #ifdef DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("Interpreter::IsArrayElement() str=%s, array=%d\n", str.c_str(), retval);
   #endif
   
   return retval;
}


//----------------------------------
// Private
//----------------------------------

//------------------------------------------------------------------------------
// AxisSystem* CreateAxisSystem(std::string type, GmatBase *owner)
//------------------------------------------------------------------------------
AxisSystem* Interpreter::CreateAxisSystem(std::string type, GmatBase *owner)
{
   if (owner == NULL)
   {
      InterpreterException ex("Interpreter::CreateAxisSystem needs a "
         "CoordinateSystem object that acts as its owner; received a NULL "
         "pointer instead.");
      HandleError(ex);
   }
   if (owner->GetType() != Gmat::COORDINATE_SYSTEM)
   {
      InterpreterException ex("Interpreter::CreateAxisSystem needs a "
         "CoordinateSystem object that acts as its owner; received a pointer "
         "to " + owner->GetName() + "instead.");
      HandleError(ex);
   }
   AxisSystem* axes = (AxisSystem *)(theModerator->CreateAxisSystem(type, ""));
   owner->SetRefObject(axes, axes->GetType(), axes->GetName());
   return axes;
}


//------------------------------------------------------------------------------
// void HandleError(const BaseException &e, bool writeLine, bool warning)
//------------------------------------------------------------------------------
void Interpreter::HandleError(const BaseException &e, bool writeLine, bool warning)
{
   if (writeLine)
   {
      lineNumber = GmatStringUtil::ToString(theReadWriter->GetLineNumber());
      currentLine = theReadWriter->GetCurrentLine();
      
      HandleErrorMessage(e, lineNumber, currentLine, writeLine, warning);
   }
   else
   {
      HandleErrorMessage(e, "", "", writeLine, warning);
   }
}


//------------------------------------------------------------------------------
// void HandleErrorMessage(const BaseException &e, const std::string &lineNumber,
//                         const std::string &line, bool warning)
//------------------------------------------------------------------------------
void Interpreter::HandleErrorMessage(const BaseException &e, const std::string &lineNumber,
                                     const std::string &line, bool writeLine,
                                     bool warning)
{
   std::string currMsg = "";
   std::string msgKind = "**** ERROR **** ";
   if (warning)
      msgKind = "*** WARNING *** ";
   
   if (writeLine)
      currMsg = " in line:\n   \"" + lineNumber + ": " + line + "\"\n";
   
   std::string msg = msgKind + e.GetFullMessage() + currMsg;
   
   if (continueOnError)
   {
      errorList.push_back(msg);
      
      #ifdef DEBUG_HANDLE_ERROR
      MessageInterface::ShowMessage(msg + "\n");
      #endif
   }
   else
   {
      if (warning)
         MessageInterface::ShowMessage(msg);
      else
         throw InterpreterException(msg);
   }
}


//------------------------------------------------------------------------------
// bool IsBranchCommand(const std::string &str)
//------------------------------------------------------------------------------
bool Interpreter::IsBranchCommand(const std::string &str)
{
   StringArray parts = theTextParser.SeparateSpaces(str);
   
   if (parts[0] == "If" || parts[0] == "EndIf" ||
       parts[0] == "For" || parts[0] == "EndFor" ||
       parts[0] == "While" || parts[0] == "EndWhile" ||
       parts[0] == "Target" || parts[0] == "EndTarget" ||
       parts[0] == "Optimize" || parts[0] == "EndOptimize" ||
       parts[0] == "BeginScript" || parts[0] == "EndScript")
      return true;
   else
      return false;
   
}


//------------------------------------------------------------------------------
// bool CheckBranchCommands(const IntegerArray &lineNumbers,
//                          const StringArray &lines,)
//------------------------------------------------------------------------------
/**
 * Checks branch command matching end command.
 *
 * @return true if the all matches, false otherwise
 */
//------------------------------------------------------------------------------
bool Interpreter::CheckBranchCommands(const IntegerArray &lineNumbers,
                                      const StringArray &lines)
{
   #ifdef DEBUG_CHECK_BRANCH
   MessageInterface::ShowMessage("Interpreter::CheckBranchCommands()\n");
   for (UnsignedInt i=0; i<lines.size(); i++)
      MessageInterface::ShowMessage("%d: %s\n", lineNumbers[i], lines[i].c_str());
   #endif
   
   // Check for unbalaced branch commands
   
   std::stack<std::string> controlStack;
   std::string expEndStr, str, str1;
   bool retval = true;
   
   #ifdef DEBUG_CHECK_BRANCH
   MessageInterface::ShowMessage("   Now start checking\n");
   #endif
   
   for (UnsignedInt i=0; i<lines.size(); i++)
   {
      str = lines[i];
      
      #ifdef DEBUG_CHECK_BRANCH
      MessageInterface::ShowMessage
         ("   line=%d, str=%s\n", lineNumbers[i], str.c_str());
      #endif
      
      if (GmatStringUtil::StartsWith(str, "End"))
      {
         if (controlStack.empty())
         {
            InterpreterException ex("Found too many \"" + str + "\"");
            HandleErrorMessage(ex, GmatStringUtil::ToString(lineNumbers[i]), str);
            retval = false;
            break;
         }
         
         str1 = controlStack.top();
         controlStack.pop();
         
         if (str1 == "BeginScript")
            expEndStr = "EndScript";
         else
            expEndStr = "End" + str1;
         
         if (expEndStr != str)
         {
            InterpreterException ex
               ("Expecting \"" + expEndStr + "\" but found \"" + str + "\"");
            HandleErrorMessage(ex, GmatStringUtil::ToString(lineNumbers[i]), str);
            retval = false;
            break;
         }
      }
      else
      {
         controlStack.push(str);
      }
   }
   
   
   if (retval == true)
   {
      if (!controlStack.empty())
      {
         InterpreterException ex
            ("Matching \"End" + controlStack.top() + "\" not found for \"" +
             controlStack.top() + "\"");
         HandleError(ex, false);
         retval = false;
      }
   }
   
   #ifdef DEBUG_CHECK_BRANCH
   MessageInterface::ShowMessage
      ("Interpreter::CheckBranchCommands() returning %d\n", retval);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool FinalPass()
//------------------------------------------------------------------------------
/**
 * Finishes up the Interpret call by setting internal references that are needed 
 * by the GUI.
 *
 * @return true if the references were set; false otherwise.
 *
 * @note: Most objects has reference objects already set in the SetObject*(),
 *        if paramter type is OBJECT_TYPE, so not requiring additional call to
 *        SetRefObject()
 */
//------------------------------------------------------------------------------
bool Interpreter::FinalPass()
{
   bool retval = true;
   
   GmatBase *obj = NULL;
   GmatBase *refObj;
   StringArray refNameList;
   std::string objName;
   StringArray objList;
   
   objList = theModerator->GetListOfObjects(Gmat::UNKNOWN_OBJECT);
   
   #ifdef DEBUG_FINAL_PASS //------------------------------ debug ----
   MessageInterface::ShowMessage("FinalPass:: All object list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif //------------------------------------------- end debug ----
   
   //----------------------------------------------------------------------
   // Check reference objects
   //----------------------------------------------------------------------
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      // Changed to call FindObject() (loj: 2008.04.01)
      //obj = GetConfiguredObject(*i);
      obj = FindObject(*i);
      
      #ifdef DEBUG_FINAL_PASS
      MessageInterface::ShowMessage
         ("Checking ref. object on %s:%s\n", obj->GetTypeName().c_str(),
          obj->GetName().c_str());
      #endif
      
      // check System Parameters seperately since it follows certain naming
      // convention.  "owner.dep.type" where owner can be either Spacecraft
      // or Burn for now
      
      if (obj->GetType() == Gmat::PARAMETER)
      {
         std::string type, owner, depObj;
         Parameter *param = (Parameter*)obj;
         
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            objName = obj->GetName();            
            GmatStringUtil::ParseParameter(objName, type, owner, depObj);
            
            // Since we can create a system parameter as: Create A1ModJulian Time,
            // we don't want to check if owner is blank.
            if (owner != "")
            {
               // Changed to call FindObject() (loj: 2008.04.01)
               //refObj = GetConfiguredObject(owner);
               refObj = FindObject(owner);
               if (refObj == NULL)
               {
                  InterpreterException ex
                     ("Nonexistent object \"" + owner + "\" referenced in \"" +
                      obj->GetName() + "\"");
                  HandleError(ex, false);
                  retval = false;
               }
               else if (param->GetOwnerType() != refObj->GetType())
               {
                  InterpreterException ex
                     ("\"" + type + "\" is not property of \"" +
                      refObj->GetTypeName() + "\"");
                  HandleError(ex, false);
                  retval = false;
               }
            }
         }
      }
      
      // check Function seperately since it has inputs that can be any object type,
      // including Real number (1234.5678) and String literal ('abc')
      
      else if (obj->GetType() == Gmat::FUNCTION)
      {
         // If GmatFunction, see if function file exist and the function name
         // matches the file name
         if (obj->GetTypeName() == "GmatFunction")
         {
            std::string funcPath = obj->GetStringParameter("FunctionPath");
            retval = CheckFunctionDefinition(funcPath, obj);
         }
      }
      
      //-----------------------------------------------------------------
      // Note: This section needs be modified as needed. 
      // GetRefObjectTypeArray() should be implemented if we want to
      // add to this list. This was added to write specific error messages.
      //-----------------------------------------------------------------
      
      else if (obj->GetType() == Gmat::BURN ||
               obj->GetType() == Gmat::SPACECRAFT ||
               obj->GetType() == Gmat::FORCE_MODEL ||
               obj->GetType() == Gmat::COORDINATE_SYSTEM ||
               obj->GetType() == Gmat::CALCULATED_POINT ||
               obj->GetType() == Gmat::SUBSCRIBER)
      {
         // Set return flag to false if any check failed
         try
         {
            bool retval1 = CheckUndefinedReference(obj, false);
            retval = retval && retval1;
            
            // Subscribers uses ElementWrapper to handle Parameter, Variable,
            // Array, Array elements, so create wrappers in ValidateSubscriber()
            if (retval && obj->GetType() == Gmat::SUBSCRIBER)
               retval = retval && ValidateSubscriber(obj);
         }
         catch (BaseException &ex)
         {
            HandleError(ex, false);
            retval = false;
         }
      }
      else
      {
         try
         {
            // Check referenced SpacePoint used by given objects
            refNameList = obj->GetRefObjectNameArray(Gmat::SPACE_POINT);
            
            for (UnsignedInt j = 0; j < refNameList.size(); j++)
            {
               // Changed to call FindObject() (loj: 2008.04.01)
               //refObj = GetConfiguredObject(refNameList[j]);
               refObj = FindObject(refNameList[j]);
               if ((refObj == NULL) || !(refObj->IsOfType(Gmat::SPACE_POINT)))
               {
                  #ifdef DEBUG_FINAL_PASS
                  MessageInterface::ShowMessage
                     ("   refNameList[%d]=%s\n", j, refNameList[j].c_str());
                  #endif
                  
                  InterpreterException ex
                     ("Nonexistent SpacePoint \"" + refNameList[j] +
                      "\" referenced in \"" + obj->GetName() + "\"");
                  HandleError(ex, false);
                  retval = false;
               }
            }
         }
         catch (BaseException &e)
         {
            #ifdef DEBUG_FINAL_PASS
            MessageInterface::ShowMessage(e.GetFullMessage());
            #endif
         }
      }
   }
   
   //-------------------------------------------------------------------
   // Special check for LibrationPoint.
   // Since the order of setting primary and secondary bodies can be
   // different, it cannot check for the same bodies in the base code
   // LibrationPoint::SetStringParameter(). Instead the checking is done
   // in here.  This allows repeated setting of bodies as shown in the
   // following script.
   //    GMAT Libration1.Primary = Sun;
   //    GMAT Libration1.Secondary = Earth;
   //    GMAT Libration1.Primary = Earth;
   //    GMAT Libration1.Secondary = Luna;
   //-------------------------------------------------------------------
   objList = theModerator->GetListOfObjects(Gmat::CALCULATED_POINT);
   
   #ifdef DEBUG_FINAL_PASS
   MessageInterface::ShowMessage("FinalPass:: CalculatedPoint list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif
   
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      // Changed to call FindObject() (loj: 2008.04.01)
      //obj = GetConfiguredObject(*i);
      obj = FindObject(*i);
      refNameList = obj->GetRefObjectNameArray(Gmat::SPACE_POINT);
      
      if (obj->GetTypeName() == "LibrationPoint")
      {
         std::string primary = obj->GetStringParameter("Primary");
         std::string secondary = obj->GetStringParameter("Secondary");
         
         #ifdef DEBUG_FINAL_PASS
         MessageInterface::ShowMessage
            ("   primary=%s, secondary=%s\n", primary.c_str(), secondary.c_str());
         #endif
         
         if (primary == secondary)
         {
            InterpreterException ex
               ("The Primay and Secondary bodies cannot be the same in the "
                "LibrationPoint \"" + obj->GetName() + "\"");
            HandleError(ex, false);
            retval = false;
         }
      }
      
      //----------------------------------------------------------------
      // Now set ref objects to CalculatedPoint objects
      //----------------------------------------------------------------
      
      #ifdef DEBUG_FINAL_PASS
      MessageInterface::ShowMessage
         ("   Setting RefObject on obj=%s\n", obj->GetName().c_str());
      #endif
      for (UnsignedInt j = 0; j < refNameList.size(); j++)
      {
         #ifdef DEBUG_FINAL_PASS
         MessageInterface::ShowMessage
            ("   refNameList[%d]=%s\n", j, refNameList[j].c_str());
         #endif
         
         // Changed to call FindObject() (loj: 2008.04.01)
         //refObj = GetConfiguredObject(refNameList[j]);
         refObj = FindObject(refNameList[j]);
         if (refObj)
            obj->SetRefObject(refObj, Gmat::SPACE_POINT, refObj->GetName());
      }
   }
   
   
   //----------------------------------------------------------------------
   // Initialize CoordinateSystem
   //----------------------------------------------------------------------
   objList = theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   
   #ifdef DEBUG_FINAL_PASS //------------------------------ debug ----
   MessageInterface::ShowMessage("FinalPass:: CoordinateSystem list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("    %s\n", (objList.at(ii)).c_str());
   #endif //------------------------------------------- end debug ----
   
   objList = theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      // Changed to call FindObject() (loj: 2008.04.01)
      //CoordinateSystem *cs = (CoordinateSystem*)GetConfiguredObject(*i);
      CoordinateSystem *cs = (CoordinateSystem*)FindObject(*i);
      #ifdef DEBUG_FINAL_PASS_CS
      MessageInterface::ShowMessage("Initializing CoordinateSystem '%s'\n",
                                    i->c_str());
      #endif
      refNameList = cs->GetRefObjectNameArray(Gmat::SPACE_POINT);
      for (UnsignedInt j = 0; j < refNameList.size(); j++)
      {
         #ifdef DEBUG_FINAL_PASS_CS
         MessageInterface::ShowMessage
            ("   refNameList[%d]=%s\n", j, refNameList[j].c_str());
         #endif
         
         // Changed to call FindObject() (loj: 2008.04.01)
         //refObj = GetConfiguredObject(refNameList[j]);
         refObj = FindObject(refNameList[j]);
         if ((refObj == NULL) || !(refObj->IsOfType(Gmat::SPACE_POINT)))
         {            
            InterpreterException ex
               ("Nonexistent SpacePoint \"" + refNameList[j] +
                "\" referenced in \"" + obj->GetName() + "\"");
            HandleError(ex, false);
            retval = false;
         }
         else
         {
            cs->SetRefObject(refObj, Gmat::SPACE_POINT, refObj->GetName());
         }
      }
      cs->Initialize();         
   }
   
   //-------------------------------------------------------------------
   // Special case for Spacecraft, we need to set CoordinateSyatem
   // pointer in which initial state is represented.  So that
   // Spacecraft can convert initial state in user representation to
   // internal representation (EarthMJ2000Eq Cartesian).
   //-------------------------------------------------------------------
   objList = theModerator->GetListOfObjects(Gmat::SPACECRAFT);

   #ifdef DEBUG_FINAL_PASS
   MessageInterface::ShowMessage("FinalPass:: Spacecraft list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif
   
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      // Changed to call FindObject() (loj: 2008.04.01)
      //obj = GetConfiguredObject(*i);
      obj = FindObject(*i);
      
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      // Changed to call FindObject() (loj: 2008.04.01)
      //GmatBase *csObj = GetConfiguredObject(csName);
      GmatBase *csObj = FindObject(csName);
      
      // To catch as many errors we can, continue with next object
      if (csObj == NULL)
         continue;
      
      #ifdef DEBUG_FINAL_PASS
      MessageInterface::ShowMessage
         ("   Calling %s->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
          csObj->GetName().c_str(), csObj, csObj->GetType());
      #endif
      
      if (csObj->GetType() != Gmat::COORDINATE_SYSTEM)
      {
         InterpreterException ex
            ("The Spacecraft \"" + obj->GetName() + "\" failed to set "
             "\"CoordinateSystem\" to \"" + csName + "\"");
         HandleError(ex, false);
         retval = false;
         continue;
      }
      
      try
      {
         obj->SetRefObject(csObj, Gmat::COORDINATE_SYSTEM, csObj->GetName());
      }
      catch (BaseException &e)
      {
         InterpreterException ex
            ("The Spacecraft \"" + obj->GetName() + "\" failed to set "
             "CoordinateSystem: " + e.GetFullMessage());
         HandleError(ex, false);
         retval = false;
         continue;
      }
   }
   
   #ifdef DEBUG_FINAL_PASS
   MessageInterface::ShowMessage("Leaving FinalPass() retval=%d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool IsObjectType(const std::string &type)
//------------------------------------------------------------------------------
/*
 * Returns true if input string is one of Object type that can be created.
 */
//------------------------------------------------------------------------------
bool Interpreter::IsObjectType(const std::string &type)
{
   if (type == "Spacecraft") 
      return true;
   
   if (type == "Formation") 
      return true;
   
   if (type == "Propagator") 
      return true;
   
   if (type == "ForceModel") 
      return true;
   
   if (type == "CoordinateSystem") 
      return true;
   
   if (theSolarSystem->IsBodyInUse(type))
      return true;
   
   if (find(propagatorList.begin(), propagatorList.end(), type) !=
       propagatorList.end())
      return true;
   
   if (find(axisSystemList.begin(), axisSystemList.end(), type) !=
       axisSystemList.end())
      return true;
   
   if (find(atmosphereList.begin(), atmosphereList.end(), type) !=
       atmosphereList.end())
      return true;
   
   if (find(attitudeList.begin(), attitudeList.end(), type) !=
       attitudeList.end())
      return true;
   
   if (find(burnList.begin(), burnList.end(), type) != burnList.end())
      return true;

   if (find(calculatedPointList.begin(), calculatedPointList.end(), type) != 
       calculatedPointList.end()) 
      return true;
   
   if (find(functionList.begin(), functionList.end(), type) != 
       functionList.end())
      return true;
   
   if (find(hardwareList.begin(), hardwareList.end(), type) != 
       hardwareList.end())
      return true;

   if (find(parameterList.begin(), parameterList.end(), type) != 
       parameterList.end())
      return true;
   
   if (find(physicalModelList.begin(), physicalModelList.end(), type) != 
       physicalModelList.end())
      return true;
   
   if (find(solverList.begin(), solverList.end(), type) != 
       solverList.end())
      return true;
   
   if (find(subscriberList.begin(), subscriberList.end(), type) != 
       subscriberList.end())
      return true;
   
   return false;
}


//------------------------------------------------------------------------------
// bool IsParameterType(const std::string &desc)
//------------------------------------------------------------------------------
/*
 * Checks if input description is a Parameter.
 * If desctiption has dots, it will parse the components into Object, Depdency,
 * and Type. If type is one of the system parameters, it will return true.
 *
 * @param  desc  Input string to check for Parameter type
 * @return  true  if type is a Parameter type
 */
//------------------------------------------------------------------------------
bool Interpreter::IsParameterType(const std::string &desc)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(desc, type, owner, dep);
   if (theModerator->IsParameter(type))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool CheckForSpecialCase(GmatBase *obj, Integer id, std::string &value)
//------------------------------------------------------------------------------
/**
 * Handles special alias for gravity field type.
 * such as JGM2, JGM3, EGM96, LP165P, etc.
 *
 * @param  obj    Pointer to the object that owns the parameter.
 * @param  id     ID for the parameter.
 * @param  value  Input/Output value of the parameter.
 */
//------------------------------------------------------------------------------
bool Interpreter::CheckForSpecialCase(GmatBase *obj, Integer id, 
                                     std::string &value)
{
   bool retval = false;
   std::string val = value;
   
   #ifdef DEBUG_SPECIAL_CASE
   MessageInterface::ShowMessage
      ("Entered CheckForSpecialCase with \"" + value +
       "\" being set on parameter \"" + obj->GetParameterText(id) + 
       "\" for a \"" + obj->GetTypeName() + "\" object\n");
   #endif
   
   // JGM2, JGM3, EGM96, LP165P, etc.  are special strings in GMAT; handle them here
   if ((obj->GetTypeName() == "GravityField") &&
       (obj->GetParameterText(id) == "PotentialFile"))
   {
      val = theModerator->GetPotentialFileName(value);
      if (val.find("Unknown Potential File Type") == std::string::npos)
      {
         value = val;
         retval = true;
      }
   }
   
   #ifdef DEBUG_SPECIAL_CASE
   MessageInterface::ShowMessage
      ("Leaving CheckForSpecialCase() value=%s, retval=%d\n", value.c_str(),
       retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool SetCommandParameter(GmatCommand *cmd, const std::string &param,
//                          const std::string &msg, bool isNumberAllowed,
//                          bool isArrayAllowed)
//------------------------------------------------------------------------------
bool Interpreter::SetCommandParameter(GmatCommand *cmd, const std::string &param,
                                      const std::string &msg, bool isNumberAllowed,
                                      bool isArrayAllowed)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetCommandParameter() cmd=%s, param=%s, isNumberAllowed=%d,"
       " isArrayAllowed=%d\n", cmd->GetTypeName().c_str(), param.c_str(),
       isNumberAllowed, isArrayAllowed);
   #endif
   
   GmatBase *obj = FindObject(param);
   Real rval;
   
   if (obj != NULL)
   {
      if ((obj->GetTypeName() == "String") ||
          ((obj->GetTypeName() != "Variable") && !isArrayAllowed))
      {
         InterpreterException ex
            (msg + " has to be \"Variable\" but found \"" +
             obj->GetTypeName() + "\"");
         HandleError(ex);
         return false;
      }
      else
      {
         if (obj->GetTypeName() == "Array" && !IsArrayElement(param))
         {
            InterpreterException ex
               (msg + " \"" + param + "\" is not a valid Array element");
            HandleError(ex);
            return false;
         }
         
         cmd->SetRefObject(obj, Gmat::PARAMETER, param);
      }
   }
   else
   {
      // Create parameter if system parameter and isNumberAllowed      
      if (isNumberAllowed)
      {
         std::string type, ownerName, depObj;
         GmatStringUtil::ParseParameter(param, type, ownerName, depObj);
         
         if (theModerator->IsParameter(type))
         {
            CreateParameter(type, param, ownerName, depObj);
            return true;
         }
      }
      
      if ((!isNumberAllowed) ||
          (isNumberAllowed && !GmatStringUtil::ToReal(param, rval, true)))
      {
         InterpreterException ex(msg + " \"" + param + "\" is undefined");
         HandleError(ex);
         return false;
      }
   }
      
   return true;
}


//------------------------------------------------------------------------------
// void WriteParts(const std::string &title, StringArray &parts)
//------------------------------------------------------------------------------
void Interpreter::WriteParts(const std::string &title, StringArray &parts)
{
   MessageInterface::ShowMessage("   ========== %s\n", title.c_str());
   for (UnsignedInt i=0; i<parts.size(); i++)
      MessageInterface::ShowMessage("   %d:%s\n", i, parts[i].c_str());
   MessageInterface::ShowMessage("\n");
}


//------------------------------------------------------------------------------
// bool CheckFunctionDefinition(const std::string &funcPath, GmatBase *function)
//------------------------------------------------------------------------------
/*
 * Opens function file and checks if it has valid function definition line.
 *
 * @param  funcPath  The full path and name of fuction file
 * @param  function  The Function pointer
 *
 */
//------------------------------------------------------------------------------
bool Interpreter::CheckFunctionDefinition(const std::string &funcPath,
                                          GmatBase *function)
{
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage
      ("Interpreter::CheckFunctionDefinition() function=<%p>,\n   funcPath=<%s>\n",
       function, funcPath.c_str());
   #endif
   
   if (function == NULL)
   {
      MessageInterface::ShowMessage
         ("** INTERNAL ERROR ** Cannot check function definition. "
          "function pointer is NULL\n");
      return false;
   }
   
   bool retval = true;
   
   // if function path exist, go through validation
   if (GmatFileUtil::DoesFileExist(funcPath))
   {
      // check for no .gmf or wrong extenstion
      StringArray parts = GmatStringUtil::SeparateBy(funcPath, ".");
      if ((parts.size() == 1) ||
          (parts.size() == 2 && parts[1] != "gmf"))
      {
         InterpreterException ex
            ("The GmatFunction file \"" + funcPath + "\" has no or incorrect file "
             "extension referenced in \"" + function->GetName() + "\"\n");
         HandleError(ex, false);
         return false;
      }
      
      // check function declaration
      std::ifstream inStream(funcPath.c_str());
      std::string line;
      
      while (!inStream.eof())
      {
         // Use global function getline()
         getline(inStream, line);
         
         #if DBGLVL_FUNCTION_DEF > 1
         MessageInterface::ShowMessage("   line=<%s>\n", line.c_str());
         #endif
         
         // Skip empty line or comment line
         if (line[0] == '\0' || line[0] == '%')
            continue;
         
         //------------------------------------------------------
         // Parse function definition line
         //------------------------------------------------------
         bool hasOutput = false;
         if (line.find("=") != line.npos)
            hasOutput = true;
         
         StringArray parts;
         if (hasOutput)
            parts = GmatStringUtil::SeparateBy(line, "=", true);
         else
            parts = GmatStringUtil::SeparateBy(line, " ", true);
         
         StringArray::size_type numParts = parts.size();
         
         #if DBGLVL_FUNCTION_DEF > 1
         WriteParts("GmatFunction parts", parts);
         #endif
         
         StringArray lhsParts;
         
         try
         {
            lhsParts = theTextParser.Decompose(parts[0], "[]", false);
         }
         catch (BaseException &e)
         {
            InterpreterException ex
               ("Invalid output argument list found in the GamtFunction file \"" +
                funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
            HandleError(ex, false);
            retval = false;
            break;
         }
         
         StringArray::size_type numLeft = lhsParts.size();
         
         #if DBGLVL_FUNCTION_DEF > 1
         WriteParts("GmatFunction lhsParts", lhsParts);
         #endif
         
         //------------------------------------------------------
         // Check if first part is "function"
         //------------------------------------------------------
         #if DBGLVL_FUNCTION_DEF > 0
         MessageInterface::ShowMessage("   Check if first part is function\n");
         #endif
         
         if (numLeft > 0 && lhsParts[0] != "function")
         {
            InterpreterException ex
               ("The \"function\" is missing in the GamtFunction file \"" +
                funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
            HandleError(ex, false);
            retval = false;
            break;
         }
         
         //------------------------------------------------------
         // Check for valid output arguments
         //------------------------------------------------------
         #if DBGLVL_FUNCTION_DEF > 0
         MessageInterface::ShowMessage("   Check for output arguments\n");
         #endif
         
         StringArray outputArgs;
         if (hasOutput)
         {
            try
            {
               outputArgs =
                  theTextParser.SeparateBrackets(lhsParts[1], "[]", ",");
               
               #if DBGLVL_FUNCTION_DEF > 1
               WriteParts("GmatFunction outputArgs", outputArgs);
               #endif
            }
            catch (BaseException &e)
            {
               InterpreterException ex
                  ("The invalid output argument list found in the GamtFunction file \"" +
                   funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
               HandleError(ex, false);
               retval = false;
               break;
            }
            
            
            if (outputArgs.size() == 0)
            {
               InterpreterException ex
                  ("The output argument list is empty in the GamtFunction file \"" +
                   funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
               HandleError(ex, false);
               retval = false;
               break;
            }
         }
         
         //------------------------------------------------------
         // Check for missing function name
         //------------------------------------------------------
         #if DBGLVL_FUNCTION_DEF > 0
         MessageInterface::ShowMessage("   Check for missing function name\n");
         MessageInterface::ShowMessage("   hasOutput=%d, numLeft=%d, numParts=%d\n",
                                       hasOutput, numLeft, numParts);
         #endif
         
         if (numParts <= 1)
         {
            InterpreterException ex
               ("The function name not found in the GamtFunction file \"" +
                funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
            HandleError(ex, false);
            retval = false;
            break;
         }
         
         //------------------------------------------------------
         // check function name and input arguments
         //------------------------------------------------------
         #if DBGLVL_FUNCTION_DEF > 0
         MessageInterface::ShowMessage("   Check for input arguments\n");
         #endif
         
         StringArray rhsParts;
         try
         {
            rhsParts = theTextParser.Decompose(parts[1], "()", false);
            
            #if DBGLVL_FUNCTION_DEF > 1
            WriteParts("GmatFunction rhsParts", rhsParts);
            #endif         
         }
         catch (BaseException &e)
         {
            InterpreterException ex
               ("The invalid input argument list found in the GamtFunction file \"" +
                funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
            HandleError(ex, false);
            retval = false;
            break;
         }
         
         //------------------------------------------------------
         // Check if function name matches the file name
         //------------------------------------------------------
         #if DBGLVL_FUNCTION_DEF > 0
         MessageInterface::ShowMessage("   Check if file has matching function name\n");
         #endif
         
         std::string fileFuncName = rhsParts[0];
         std::string funcName = function->GetStringParameter("FunctionName");
         
         #if DBGLVL_FUNCTION_DEF > 0
         MessageInterface::ShowMessage
            ("   fileFuncName=<%s>, funcName=<%s>\n", fileFuncName.c_str(), funcName.c_str());
         #endif
         
         if (fileFuncName != funcName)
         {
            InterpreterException ex
               ("The function name \"" + fileFuncName + "\" does not match with the"
                "GmatFunction file name \"" + funcPath + "\" referenced in \"" +
                function->GetName() + "\"\n");
            HandleError(ex, false);
            retval = false;
         }
         
         //------------------------------------------------------
         // Check for valid input arguments
         //------------------------------------------------------
         #if DBGLVL_FUNCTION_DEF > 0
         MessageInterface::ShowMessage("   Check for input arguments\n");
         #endif
         if (rhsParts.size() > 1)
         {
            StringArray inputArgs;
            try
            {
               inputArgs =
                  theTextParser.SeparateBrackets(rhsParts[1], "()", ",");
               
               #if DBGLVL_FUNCTION_DEF > 1
               WriteParts("GmatFunction inputArgs", inputArgs);
               #endif
            }
            catch (BaseException &e)
            {
               InterpreterException ex
                  ("Invalid input argument list found in the GamtFunction file \"" +
                   funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
               HandleError(ex, false);
               retval = false;
               break;
            }
            
            if (inputArgs.size() == 0)
            {
               InterpreterException ex
                  ("The input argument list is empty in the GamtFunction file \"" +
                   funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
               HandleError(ex, false);
               retval = false;
               break;
            }
         }
         
         break;
      }
      
      if (line == "")
      {
         InterpreterException ex
            ("The GmatFunction file \"" + funcPath + "\" referenced in \"" +
             function->GetName() + "\" is empty\n");
         HandleError(ex, false);
         retval = false;
      }
      
      inStream.close();
   }
   else
   {
      InterpreterException ex
         ("Nonexistent GmatFunction file \"" + funcPath +
          "\" referenced in \"" + function->GetName() + "\"\n");
      HandleError(ex, false);
      retval = false;
   }
   
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage
      ("Interpreter::CheckFunctionDefinition() returning true\n");
   #endif
   
   return retval;
   
} // CheckFunctionDefinition()


//------------------------------------------------------------------------------
// bool BuildFunctionDefinition(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Sets function inputs and output to function from valid function definition
 * string.
 *
 * Note: This methods assumes that input string already has passed function
 *       validation check
 */
//------------------------------------------------------------------------------
bool Interpreter::BuildFunctionDefinition(const std::string &str)
{
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage
      ("Interpreter::BuildFunctionDefinition() str=<%s>\n", str.c_str());
   #endif
   
   std::string lhs;
   std::string rhs;
   StringArray parts = theTextParser.SeparateBy(str, "=");
   
   #if DBGLVL_FUNCTION_DEF > 1
   WriteParts("parts", parts);
   #endif
   
   // if function has no output
   if (parts.size() == 1)
   {
      std::string::size_type index = str.find_first_of(" ");
      lhs = str.substr(0, index);
      rhs = str.substr(index+1);
   }
   else
   {
      lhs = parts[0];
      rhs = parts[1];
   }
   
   StringArray lhsParts = theTextParser.Decompose(lhs, "[]", false);
   StringArray rhsParts = theTextParser.Decompose(rhs, "()", false);
   
   #if DBGLVL_FUNCTION_DEF > 1
   WriteParts("lhsParts", lhsParts);
   WriteParts("rhsParts", rhsParts);
   #endif
   
   std::string funcName;
   
   if (lhsParts[0] != "function")
      return false;
   
   if (!GmatStringUtil::IsValidName(rhsParts[0], false))
      return false;
   
   StringArray inputs, outputs;
   
   //------------------------------------------------------
   // parse inputs
   //------------------------------------------------------
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage("   parse inputs\n");
   #endif
   
   if (rhsParts.size() > 1)
   {
      inputs = theTextParser.SeparateBy(rhsParts[1], ", ()");
      
      #if DBGLVL_FUNCTION_DEF > 1
      WriteParts("function inputs", inputs);
      #endif
   }
   
   //------------------------------------------------------
   // parse outputs
   //------------------------------------------------------
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage("   parse outputs\n");
   #endif
   if (lhsParts.size() > 1)
   {
      outputs = theTextParser.SeparateBy(lhsParts[1], ", []");
      
      #if DBGLVL_FUNCTION_DEF > 1
      WriteParts("function outputs", outputs);
      #endif
   }
   
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage
      ("   inFunctionMode=%d, currentFunction=<%p>\n", inFunctionMode,
       currentFunction);
   #endif
   
   //------------------------------------------------------
   // set inputs and outputs to current function
   //------------------------------------------------------
   if (inFunctionMode && currentFunction != NULL)
   {
      for (UnsignedInt i=0; i<inputs.size(); i++)
         currentFunction->SetStringParameter("Input", inputs[i]);

      for (UnsignedInt i=0; i<outputs.size(); i++)
         currentFunction->SetStringParameter("Output", outputs[i]);
   }
   
   hasFunctionDefinition = true;
   
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage
      ("Interpreter::BuildFunctionDefinition() returning true\n");
   #endif
   
   return true;
   
} // BuildFunctionDefinition()


