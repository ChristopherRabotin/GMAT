//$Id$
//------------------------------------------------------------------------------
//                                  Interpreter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/08/28
// Rework:  2006/09/27 by Linda Jun (NASA/GSFC)
// Modified: 
//    2010.03.24 Thomas Grubb
//      - Fixed FinalPass method to gracefully print error message instead of 
//        access violation when celestial body is not set properly
//    2010.03.23 Thomas Grubb/Steve Hughes
//      - Fixed error message in SetPropertyToValue for invalid field values
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class implementation for the Interpreter base class
 */
//------------------------------------------------------------------------------

#include "Interpreter.hpp"          // class's header file
#include "Moderator.hpp"
#include "StringTokenizer.hpp"
#include "ConditionalBranch.hpp"
#include "SolverBranchCommand.hpp"  // To check Vary command levels
#include "StringUtil.hpp"           // for ToReal()
#include "TimeTypes.hpp"
#include "Array.hpp"
#include "Assignment.hpp"           // for GetLHS(), GetRHS()
#include "Validator.hpp"
#include "ElementWrapper.hpp"
#include "MessageInterface.hpp"
#include "FileUtil.hpp"             // for DoesFileExist()
#include "GmatGlobal.hpp"           // for GetMatlabFuncNameExt()
#include "Covariance.hpp"

#include <stack>                    // for checking matching begin/end control logic
#include <fstream>                  // for checking GmatFunction declaration
#include <sstream>                  // for checking GmatFunction declaration

//#define __DO_NOT_USE_OBJ_TYPE_NAME__

//#define DEBUG_HANDLE_ERROR
//#define DEBUG_INIT
//#define DEBUG_COMMAND_LIST
//#define DEBUG_COMMAND_VALIDATION
//#define DEBUG_OBJECT_LIST
//#define DEBUG_ARRAY_GET
//#define DEBUG_CREATE_OBJECT
//#define DEBUG_CREATE_CELESTIAL_BODY
//#define DEBUG_CREATE_PARAM
//#define DEBUG_CREATE_ARRAY
//#define DEBUG_CREATE_COMMAND
//#define DEBUG_CREATE_CALL_FUNCTION
//#define DEBUG_VALIDATE_COMMAND
//#define DEBUG_WRAPPERS
//#define DEBUG_MAKE_ASSIGNMENT
//#define DEBUG_ASSEMBLE_COMMAND
//#define DEBUG_ASSEMBLE_CREATE
//#define DEBUG_ASSEMBLE_CONDITION
//#define DEBUG_ASSEMBLE_FOR
//#define DEBUG_ASSEMBLE_CALL_FUNCTION
//#define DEBUG_ASSEMBLE_REPORT_COMMAND
//#define DEBUG_SET
//#define DEBUG_SET_FORCE_MODEL
//#define DEBUG_SET_SOLAR_SYS
//#define DEBUG_CHECK_OBJECT
//#define DEBUG_CHECK_BRANCH
//#define DEBUG_SPECIAL_CASE
//#define DEBUG_PARSE_REPORT
//#define DEBUG_OBJECT_MAP
//#define DEBUG_OBJECT_LIST
//#define DEBUG_FIND_OBJECT
//#define DEBUG_FIND_PROP_ID
//#define DEBUG_VAR_EXPRESSION
//#define DEBUG_MATH_TREE
//#define DEBUG_FUNCTION
//#define DBGLVL_FUNCTION_DEF 2
//#define DBGLVL_FINAL_PASS 2
//#define DEBUG_AXIS_SYSTEM
//#define DEBUG_SET_MEASUREMENT_MODEL
//#define DEBUG_ALL_OBJECTS

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

StringArray Interpreter::allObjectTypeList = StringArray(1, "");
StringArray Interpreter::viewableCommandList = StringArray(1, "");
std::map<std::string, Gmat::ObjectType> Interpreter::objectTypeMap;

const std::string Interpreter::defaultIndicator = "DFLT__";


//------------------------------------------------------------------------------
// Interpreter(SolarSystem *ss = NULL, ObjectMap *objMap = NULL)
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param  ss  The solar system to be used for finding bodies
 * @param  objMap  The object map to be used for finding object 
 */
//------------------------------------------------------------------------------
Interpreter::Interpreter(SolarSystem *ss, ObjectMap *objMap)
{
   inCommandMode = false;
   inRealCommandMode = false;
   beginMissionSeqFound = false;
   initialized = false;
   continueOnError = true;
   parsingDelayedBlock = false;
   ignoreError = false;
   inScriptEvent = false;
   gmatFunctionsAvailable = false;
   inFunctionMode = false;
   hasFunctionDefinition = false;
   currentFunction = NULL;
   theSolarSystem = NULL;
   theObjectMap = NULL;
   currentBlockType = Gmat::COMMENT_BLOCK;      // Initialize to something here
   
   theModerator  = Moderator::Instance();
   theReadWriter = ScriptReadWriter::Instance();
   theValidator = Validator::Instance();
   // Set Interpreter to singleton Validator
   theValidator->SetInterpreter(this);
   
   if (ss)
   {
      theSolarSystem = ss;
      theValidator->SetSolarSystem(ss);
   }
   
   if (objMap)
   {
      theObjectMap = objMap;
      theValidator->SetObjectMap(objMap);
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Interpreter setting object map <%p> to Validator\n", theObjectMap);
      #endif
   }
   
   #ifdef DEBUG_INTERP
   MessageInterface::ShowMessage
      ("Interpreter::Interpreter() initialized=%d, theModerator=%p, theReadWriter=%p, "
       "theValidator=%p\n", initialized, theModerator, theReadWriter, theValidator);
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
      ("Interpreter::Initialize() entered, initialized=%d\n", initialized);
   #endif
   
   errorList.clear();
   delayedBlocks.clear();
   delayedBlockLineNumbers.clear();
   inCommandMode = false;
   parsingDelayedBlock = false;
   ignoreError = false;
   
   if (initialized)
   {
      #ifdef DEBUG_INIT
      MessageInterface::ShowMessage
         ("Interpreter::Initialize() already initialized so just returning\n");
      #endif
      return;
   }
   
   BuildCreatableObjectMaps();
   
   // Register aliases used in scripting.  Plugins cannot use aliases, so this 
   // piece is performed outside of the creatable object map definitions.
   RegisterAliases();
   
   // Initialize TextParser command list
   theTextParser.Initialize(commandList);

   initialized = true;
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("Interpreter::Initialize() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void BuildCreatableObjectMaps()
//------------------------------------------------------------------------------
/**
 * Constructs the lists of object type names available in the Factories.  
 * 
 * This method is called whenever factories are registered with the 
 * FactoryManager.  During system startup, the Moderator makes this call after 
 * registering the default factories.  The call is reissued whenever a user
 * created factory is registered using the plug-in interfaces.
 */
//------------------------------------------------------------------------------
void Interpreter::BuildCreatableObjectMaps()
{
   // Build a mapping for all of the defined commands
   commandList.clear();
   StringArray cmds = theModerator->GetListOfFactoryItems(Gmat::COMMAND);
   copy(cmds.begin(), cmds.end(), back_inserter(commandList));
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("\nNumber of commands = %d\n", cmds.size());
   #endif
   
   #ifdef DEBUG_COMMAND_LIST
      std::vector<std::string>::iterator pos1;
      MessageInterface::ShowMessage("Commands:\n");      
      for (pos1 = cmds.begin(); pos1 != cmds.end(); ++pos1)
         MessageInterface::ShowMessage("   " + *pos1 + "\n");
   #endif
      
   if (cmds.size() == 0)
   {
      throw InterpreterException("Command list is empty.");
   }
   
   // Build a mapping for all viewable commands via GUI
   viewableCommandList.clear();
   cmds = theModerator->GetListOfViewableItems(Gmat::COMMAND);
   copy(cmds.begin(), cmds.end(), back_inserter(viewableCommandList));
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("\nNumber of viewable commands = %d\n", cmds.size());
   #endif
   
   #ifdef DEBUG_COMMAND_LIST
   std::vector<std::string>::iterator pos;
   MessageInterface::ShowMessage("Viewable Commands:\n");      
   for (pos = cmds.begin(); pos != cmds.end(); ++pos)
      MessageInterface::ShowMessage("   " + *pos + "\n");   
   #endif
   
   // Build a mapping for all of the defined objects
   allObjectTypeList.clear();
   celestialBodyList.clear();
   objectTypeMap.clear();
   
   StringArray scs = theModerator->GetListOfFactoryItems(Gmat::SPACECRAFT);
   //copy(scs.begin(), scs.end(), back_inserter(spacecraftList));
   copy(scs.begin(), scs.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < scs.size(); i++)
      objectTypeMap.insert(std::make_pair(scs[i], Gmat::SPACECRAFT));
   
   StringArray cbs = theModerator->GetListOfFactoryItems(Gmat::CELESTIAL_BODY);
   copy(cbs.begin(), cbs.end(), back_inserter(celestialBodyList));
   copy(cbs.begin(), cbs.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < cbs.size(); i++)
      objectTypeMap.insert(std::make_pair(cbs[i], Gmat::CELESTIAL_BODY));
   
   atmosphereList.clear();
   StringArray atms = theModerator->GetListOfFactoryItems(Gmat::ATMOSPHERE);
   copy(atms.begin(), atms.end(), back_inserter(atmosphereList));
   copy(atms.begin(), atms.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < atmosphereList.size(); i++)
      objectTypeMap.insert(std::make_pair(atmosphereList[i], Gmat::ATMOSPHERE));
   
   attitudeList.clear();
   StringArray atts = theModerator->GetListOfFactoryItems(Gmat::ATTITUDE);
   copy(atts.begin(), atts.end(), back_inserter(attitudeList));
   copy(atts.begin(), atts.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < attitudeList.size(); i++)
      objectTypeMap.insert(std::make_pair(attitudeList[i], Gmat::ATTITUDE));
   
   axisSystemList.clear();
   StringArray axes = theModerator->GetListOfFactoryItems(Gmat::AXIS_SYSTEM);
   copy(axes.begin(), axes.end(), back_inserter(axisSystemList));
   copy(axes.begin(), axes.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < axisSystemList.size(); i++)
      objectTypeMap.insert(std::make_pair(axisSystemList[i], Gmat::AXIS_SYSTEM));
   
   burnList.clear();
   StringArray burns = theModerator->GetListOfFactoryItems(Gmat::BURN);
   copy(burns.begin(), burns.end(), back_inserter(burnList));
   copy(burns.begin(), burns.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < burnList.size(); i++)
      objectTypeMap.insert(std::make_pair(burnList[i], Gmat::BURN));
   
   calculatedPointList.clear();
   StringArray cals = theModerator->GetListOfFactoryItems(Gmat::CALCULATED_POINT);
   copy(cals.begin(), cals.end(), back_inserter(calculatedPointList));
   copy(cals.begin(), cals.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < calculatedPointList.size(); i++)
      objectTypeMap.insert(std::make_pair(calculatedPointList[i], Gmat::CALCULATED_POINT));
   
   dataFileList.clear();
   StringArray dfs = theModerator->GetListOfFactoryItems(Gmat::DATA_FILE);
   copy(dfs.begin(), dfs.end(), back_inserter(dataFileList));
   copy(dfs.begin(), dfs.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < dataFileList.size(); i++)
      objectTypeMap.insert(std::make_pair(dataFileList[i], Gmat::DATA_FILE));
   
   ephemFileList.clear();
   StringArray ephems = theModerator->GetListOfFactoryItems(Gmat::EPHEMERIS_FILE);
   copy(ephems.begin(), ephems.end(), back_inserter(ephemFileList));
   copy(ephems.begin(), ephems.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < ephemFileList.size(); i++)
      objectTypeMap.insert(std::make_pair(ephemFileList[i], Gmat::EPHEMERIS_FILE));
   
   functionList.clear();
   StringArray fns = theModerator->GetListOfFactoryItems(Gmat::FUNCTION);
   copy(fns.begin(), fns.end(), back_inserter(functionList));
   copy(fns.begin(), fns.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < functionList.size(); i++)
      objectTypeMap.insert(std::make_pair(functionList[i], Gmat::FUNCTION));
   gmatFunctionsAvailable =
         (find(fns.begin(), fns.end(), "GmatFunction") != fns.end());
   
   hardwareList.clear();
   StringArray hws = theModerator->GetListOfFactoryItems(Gmat::HARDWARE);
   copy(hws.begin(), hws.end(), back_inserter(hardwareList));
   copy(hws.begin(), hws.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < hardwareList.size(); i++)
      objectTypeMap.insert(std::make_pair(hardwareList[i], Gmat::HARDWARE));
   
   measurementList.clear();
   StringArray measurements = theModerator->GetListOfFactoryItems(Gmat::CORE_MEASUREMENT);
   copy(measurements.begin(), measurements.end(), back_inserter(measurementList));
   copy(measurements.begin(), measurements.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < measurementList.size(); i++)
      objectTypeMap.insert(std::make_pair(measurementList[i], Gmat::CORE_MEASUREMENT));
   
   obtypeList.clear();
   StringArray obs = theModerator->GetListOfFactoryItems(Gmat::OBTYPE);
   copy(obs.begin(), obs.end(), back_inserter(obtypeList));
   copy(obs.begin(), obs.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < obtypeList.size(); i++)
      objectTypeMap.insert(std::make_pair(obtypeList[i], Gmat::OBTYPE));
   
   odeModelList.clear();
   StringArray odes = theModerator->GetListOfFactoryItems(Gmat::ODE_MODEL);
   copy(odes.begin(), odes.end(), back_inserter(odeModelList));
   copy(odes.begin(), odes.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < odeModelList.size(); i++)
      objectTypeMap.insert(std::make_pair(odeModelList[i], Gmat::ODE_MODEL));
   
   parameterList.clear();
   StringArray parms = theModerator->GetListOfFactoryItems(Gmat::PARAMETER);
   copy(parms.begin(), parms.end(), back_inserter(parameterList));
   copy(parms.begin(), parms.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < parameterList.size(); i++)
      objectTypeMap.insert(std::make_pair(parameterList[i], Gmat::PARAMETER));
   
   propagatorList.clear();
   StringArray props = theModerator->GetListOfFactoryItems(Gmat::PROPAGATOR);
   copy(props.begin(), props.end(), back_inserter(propagatorList));
   copy(props.begin(), props.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < propagatorList.size(); i++)
      objectTypeMap.insert(std::make_pair(propagatorList[i], Gmat::PROPAGATOR));
   
   physicalModelList.clear();
   StringArray forces = theModerator->GetListOfFactoryItems(Gmat::PHYSICAL_MODEL);
   copy(forces.begin(), forces.end(), back_inserter(physicalModelList));
   copy(forces.begin(), forces.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < physicalModelList.size(); i++)
      objectTypeMap.insert(std::make_pair(physicalModelList[i], Gmat::PHYSICAL_MODEL));
   
   solverList.clear();
   StringArray solvers = theModerator->GetListOfFactoryItems(Gmat::SOLVER);
   copy(solvers.begin(), solvers.end(), back_inserter(solverList));
   copy(solvers.begin(), solvers.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < solverList.size(); i++)
      objectTypeMap.insert(std::make_pair(solverList[i], Gmat::SOLVER));
   
   stopcondList.clear();
   StringArray stops = theModerator->GetListOfFactoryItems(Gmat::STOP_CONDITION);
   copy(stops.begin(), stops.end(), back_inserter(stopcondList));
   copy(stops.begin(), stops.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < stopcondList.size(); i++)
      objectTypeMap.insert(std::make_pair(stopcondList[i], Gmat::STOP_CONDITION));
   
   subscriberList.clear();
   StringArray subs = theModerator->GetListOfFactoryItems(Gmat::SUBSCRIBER);
   copy(subs.begin(), subs.end(), back_inserter(subscriberList));
   copy(subs.begin(), subs.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < subscriberList.size(); i++)
      objectTypeMap.insert(std::make_pair(subscriberList[i], Gmat::SUBSCRIBER));
   
   spacePointList.clear();
   StringArray spl = theModerator->GetListOfFactoryItems(Gmat::SPACE_POINT);
   copy(spl.begin(), spl.end(), back_inserter(spacePointList));
   copy(spl.begin(), spl.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < spacePointList.size(); i++)
      objectTypeMap.insert(std::make_pair(spacePointList[i], Gmat::SPACE_POINT));
   
   trackingSystemList.clear();
   StringArray tsl = theModerator->GetListOfFactoryItems(Gmat::TRACKING_SYSTEM);
   copy(tsl.begin(), tsl.end(), back_inserter(trackingSystemList));
   copy(tsl.begin(), tsl.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < trackingSystemList.size(); i++)
      objectTypeMap.insert(std::make_pair(trackingSystemList[i], Gmat::TRACKING_SYSTEM));
   
   eventLocatorList.clear();
   StringArray ell = theModerator->GetListOfFactoryItems(Gmat::EVENT_LOCATOR);
   copy(ell.begin(), ell.end(), back_inserter(eventLocatorList));
   copy(ell.begin(), ell.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < eventLocatorList.size(); i++)
      objectTypeMap.insert(std::make_pair(eventLocatorList[i], Gmat::EVENT_LOCATOR));

   interfaceList.clear();
   StringArray itf = theModerator->GetListOfFactoryItems(Gmat::INTERFACE);
   copy(itf.begin(), itf.end(), back_inserter(interfaceList));
   copy(itf.begin(), itf.end(), back_inserter(allObjectTypeList));
   for (UnsignedInt i = 0; i < interfaceList.size(); i++)
      objectTypeMap.insert(std::make_pair(interfaceList[i], Gmat::INTERFACE));

   #ifdef DEBUG_OBJECT_LIST
      std::vector<std::string>::iterator pos;
      
      MessageInterface::ShowMessage("\nSpacecraft:\n   ");
      for (pos = scs.begin(); pos != scs.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
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
      
      MessageInterface::ShowMessage("\nDataFiles:\n   ");
      for (pos = dfs.begin(); pos != dfs.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nEphemerisFiles:\n   ");
      for (pos = ephems.begin(); pos != ephems.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nFunctions:\n   ");
      for (pos = fns.begin(); pos != fns.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nHardwares:\n   ");
      for (pos = hws.begin(); pos != hws.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nODEModels:\n   ");
      for (pos = odes.begin(); pos != odes.end(); ++pos)
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
      
      MessageInterface::ShowMessage("\nMeasurements:\n   ");
      for (pos = measurements.begin();
            pos != measurements.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nObservations:\n   ");
      for (pos = obs.begin();
            pos != obs.end(); ++pos)
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
      
      MessageInterface::ShowMessage("\nTrackingSystems:\n   ");
      for (pos = tsl.begin(); pos != tsl.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("\nInterfaces:\n   ");
      for (pos = itf.begin(); pos != itf.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");

      MessageInterface::ShowMessage("\nOther SpacePoints:\n   ");
      for (pos = spl.begin(); pos != spl.end(); ++pos)
         MessageInterface::ShowMessage(*pos + "\n   ");
      
      MessageInterface::ShowMessage("GmatFunctions %s available\n",
            gmatFunctionsAvailable ? "are" : "are not");

      MessageInterface::ShowMessage("\n");
   #endif
   
   // Update the Parameter list in the Validator
   theValidator->UpdateLists();
}


//------------------------------------------------------------------------------
// StringArray GetCreatableList(Gmat::ObjectType type, Integer subType)
//------------------------------------------------------------------------------
/**
 * Returns the list of objects of a given type that can be built.
 * 
 * This method returns the list of object types supported by the current Factory 
 * system.  A future build will allow specification of a subtype -- for example,
 * for solvers, subtypes could be targeters, optimizers, iterators, and 
 * odSolvers.  The subType parameter is included to support this feature when it
 * becomes available.
 * 
 * @param type The Gmat::ObjectType requested.
 * @param subType The subtype.
 * 
 * @return The list of creatable objects.
 * 
 * @note The current implementation only supports the types in the Interpreter's
 *       lists of objects.  A future implementation should call 
 *       Moderator::GetListOfFactoryItems() instead. 
 */
//------------------------------------------------------------------------------
StringArray Interpreter::GetCreatableList(Gmat::ObjectType type, 
      const std::string subType)
{
   StringArray clist;
   
   switch (type)
   {
      case Gmat::CELESTIAL_BODY:
         clist = celestialBodyList;
         break;
      
      case Gmat::ATMOSPHERE:
         clist = atmosphereList;
         break;
         
      case Gmat::ATTITUDE:
         clist = attitudeList;
         break;
         
      case Gmat::AXIS_SYSTEM:
         clist = axisSystemList;
         break;
         
      case Gmat::BURN:
         clist = burnList;
         break;
         
      case Gmat::CALCULATED_POINT:
         clist = calculatedPointList;
         break;
         
      case Gmat::COMMAND:
         clist = commandList;
         break;
         
      case Gmat::DATA_FILE:
         clist = dataFileList;
         break;
         
      case Gmat::EVENT_LOCATOR:
         clist = eventLocatorList;
         break;

      case Gmat::FUNCTION:
         clist = functionList;
         break;
         
      case Gmat::HARDWARE:
         clist = hardwareList;
         break;
         
      case Gmat::CORE_MEASUREMENT:
         clist = measurementList;
         break;

      case Gmat::OBTYPE:
         clist = obtypeList;
         break;

      case Gmat::ODE_MODEL:
         clist = odeModelList;
         break;
         
      case Gmat::PARAMETER:
         clist = parameterList;
         break;
         
      case Gmat::PROPAGATOR:
         clist = propagatorList;
         break;
         
      case Gmat::PHYSICAL_MODEL:
         clist = physicalModelList;
         break;
         
      case Gmat::SOLVER:
         clist = solverList;
         break;
         
      case Gmat::STOP_CONDITION:
         clist = stopcondList;
         break;
         
      case Gmat::SUBSCRIBER:
         clist = subscriberList;
         break;
         
      case Gmat::SPACE_POINT:
         clist = spacePointList;
         break;
         
      case Gmat::TRACKING_SYSTEM:
         clist = trackingSystemList;
         break;
         
      case Gmat::INTERFACE:
         clist = interfaceList;
         break;

      // These are all intentional fall-throughs:
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
      case Gmat::SPACEOBJECT:
      case Gmat::GROUND_STATION:
      case Gmat::IMPULSIVE_BURN:
      case Gmat::FINITE_BURN:
      case Gmat::TRANSIENT_FORCE:
      case Gmat::INTERPOLATOR:
      case Gmat::SOLAR_SYSTEM:
//      case Gmat::CELESTIAL_BODY:
      case Gmat::LIBRATION_POINT:
      case Gmat::BARYCENTER:
      case Gmat::PROP_SETUP:
      case Gmat::FUEL_TANK:
      case Gmat::THRUSTER:
      case Gmat::COORDINATE_SYSTEM:
      case Gmat::MATH_NODE:
      case Gmat::MATH_TREE:
      case Gmat::MEASUREMENT_MODEL:
      case Gmat::DATASTREAM:
      case Gmat::TRACKING_DATA:
      case Gmat::UNKNOWN_OBJECT:
      default:
         break;
   }
   
   if (subType != "")
   {
      #ifdef DEBUG_SUBTYPES
         MessageInterface::ShowMessage("List has %d members:\n", clist.size());
         for (UnsignedInt j = 0; j < clist.size(); ++j)
            MessageInterface::ShowMessage("   %s\n", clist[j].c_str());
      #endif

      StringArray temp;
      // Throw away objects that do not match the subtype
      for (UnsignedInt i = 0; i < clist.size(); ++i)
      {
         if (theModerator->DoesObjectTypeMatchSubtype(type, clist[i], subType))
            temp.push_back(clist[i]);
      }
      clist = temp;

      #ifdef DEBUG_SUBTYPES
         MessageInterface::ShowMessage("Revised list has %d members:\n", clist.size());
         for (UnsignedInt j = 0; j < clist.size(); ++j)
            MessageInterface::ShowMessage("   %s\n", clist[j].c_str());
      #endif
   }

   return clist;
}

//------------------------------------------------------------------------------
// void SetInputFocus()
//------------------------------------------------------------------------------
/*
 * Some GMAT UiInterpreters need to be able to obtain focus for message 
 * processing.  This method is overridden to perform run complete actions for 
 * those interpreters.
 */
//------------------------------------------------------------------------------
void Interpreter::SetInputFocus()
{}

//------------------------------------------------------------------------------
// void NotifyRunCompleted()
//------------------------------------------------------------------------------
/*
 * Some GMAT UiInterpreters need to know when a run is finished.  This method is
 * overridden to perform run complete actions for those interpreters.
 */
//------------------------------------------------------------------------------
void Interpreter::NotifyRunCompleted()
{} 

//------------------------------------------------------------------------------
// void NotifyRunCompleted(Integer type)
//------------------------------------------------------------------------------
/*
 * Some GMAT UiInterpreters need to update their view into the configured 
 * objects.  This method is overridden to perform those updates.  The parameter
 * maps to the following values:
 * 
 *  1   Configured objects
 *  2   Commands
 *  3   Commands and configured objects
 *  4   Outputs
 *  5   Outputs and configured objects
 *  6   Commands and Outputs
 *  7   Everything (Commands, outputs, configured objects)
 * 
 * The default value is 7. 
 */
//------------------------------------------------------------------------------
void Interpreter::UpdateView(Integer type)
{}


//------------------------------------------------------------------------------
// void SetInputFocus()
//------------------------------------------------------------------------------
/*
 * Some GMAT UiInterpreters need to take actions when a project is closed.  This
 * method tells them to take those actions.
 */
//------------------------------------------------------------------------------
void Interpreter::CloseCurrentProject()
{}

//------------------------------------------------------------------------------
// void StartMatlabServer()
//------------------------------------------------------------------------------
/*
 * Some GMAT Interpreters can start external servers -- for example, the MATLAB
 * server.  This method is overridden to perform that startup.
 */
//------------------------------------------------------------------------------
void Interpreter::StartMatlabServer()
{
   throw InterpreterException(
         "This Interpreter cannot start the external server");
}

//------------------------------------------------------------------------------
// Interface* GetMatlabInterface()
//------------------------------------------------------------------------------
Interface* Interpreter::GetMatlabInterface()
{
   return theModerator->GetMatlabInterface();
}

//------------------------------------------------------------------------------
// bool OpenMatlabEngine()
//------------------------------------------------------------------------------
bool Interpreter::OpenMatlabEngine()
{
   return theModerator->OpenMatlabEngine();
}

//------------------------------------------------------------------------------
// bool CloseMatlabEngine()
//------------------------------------------------------------------------------
bool Interpreter::CloseMatlabEngine()
{
   return theModerator->CloseMatlabEngine();
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
   ODEModel::SetScriptAlias("PrimaryBodies", "GravityField");
   ODEModel::SetScriptAlias("Gravity", "GravityField");
   ODEModel::SetScriptAlias("PointMasses", "PointMassForce");
   ODEModel::SetScriptAlias("Drag", "DragForce");
   ODEModel::SetScriptAlias("SRP", "SolarRadiationPressure");
   ODEModel::SetScriptAlias("PolyhedralBodies", "PolyhedronGravityModel");
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
// const StringArray& GetListOfObjects(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of given object type name.
 *
 * @param  typeName  object type name
 *
 * @return array of configured item names; return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& Interpreter::GetListOfObjects(const std::string &typeName)
{
   return theModerator->GetListOfObjects(typeName);
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfViewableCommands();
//------------------------------------------------------------------------------
/**
 * Returns names of all viewable commands via GUI
 * @return array of item names; return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& Interpreter::GetListOfViewableCommands()
{
   return Interpreter::viewableCommandList;
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfViewableSubtypesOf(Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& Interpreter::GetListOfViewableSubtypesOf(Gmat::ObjectType type)
{
   return theModerator->GetListOfViewableItems(type);
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
//                        Integer manage, bool createDefault)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to build core objects and put them in the ConfigManager.
 *  
 * @param  type  Type for the requested object.
 * @param  name  Name for the object
 * @param  manage   0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 * @param <createDefault> set to true if default object to be created (false)
 *
 * @return object pointer on success, NULL on failure.
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::CreateObject(const std::string &type,
                                    const std::string &name,
                                    Integer manage, bool createDefault,
                                    bool includeLineOnError)
{
   #ifdef DEBUG_CREATE_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::CreateObject() type=<%s>, name=<%s>, manage=%d, createDefault=%d, "
       "includeLineOnError=%d\n   continueOnError=%d, inCommandMode=%d, currentFunction=<%p>\n",
       type.c_str(), name.c_str(), manage, createDefault, includeLineOnError, continueOnError,
       inCommandMode, currentFunction);
   #endif
   
   debugMsg = "In CreateObject()";
   GmatBase *obj = NULL;
   
   // if object to be managed and has non-blank name, and name is not valid, handle error
   if (manage == 1 && name != "")
   {
      bool isValid = false;
      
      // if type is Array, set flag to ignore bracket
      if (type == "Array")
         isValid = GmatStringUtil::IsValidName(name, true);
      else
         isValid = GmatStringUtil::IsValidName(name, false);
      
      if (!isValid)
      {
         #ifdef DEBUG_CREATE_OBJECT
         MessageInterface::ShowMessage
            ("Object name %s is NOT valid\n", name.c_str());
         #endif
         InterpreterException ex
            (type + " object can not be named to \"" + name + "\"");
         HandleError(ex, includeLineOnError);
         return NULL;
      }
   }
   
   // Go through more checking if name is not blank
   if (name != "")
   {
      // object name cannot be any of command names
      if (IsCommandType(name))
      {
         InterpreterException ex
            (type + " object can not be named to a Command type \"" + name + "\"");
         HandleError(ex, includeLineOnError);
         return NULL;
      }
      
      #ifdef __DO_NOT_USE_OBJ_TYPE_NAME__
      // object name cannot be any of object types
      if (IsObjectType(name))
      {
         InterpreterException ex
            (type + " object can not be named to an Object Type \"" + name + "\"");
         HandleError(ex, includeLineOnError);
         return NULL;
      }
      #endif
      
      // If object to be managed, give warning if name already exist
      if (manage == 1)
      {
//         if ((name != "EarthMJ2000Eq") &&
//             (name != "EarthMJ2000Ec") &&
//             (name != "EarthFixed")    &&
//             (name != "EarthICRF"))
//         {
            obj = FindObject(name);
            // Since System Parameters are created automatically as they are referenced,
            // do not give warning if creating a system parameter
            if (obj != NULL && ((obj->GetType() != Gmat::PARAMETER) ||
                                (obj->GetType() == Gmat::PARAMETER &&
                                 (!obj->IsOfType("SystemParameter")))))
            {
               InterpreterException ex("");
               ex.SetDetails("%s object named \"%s\" already exists",
                             type.c_str(), name.c_str());
               HandleError(ex, true, true);
               return obj;
            }
//         }
      }
   }
   #ifdef DEBUG_CREATE_CELESTIAL_BODY
   MessageInterface::ShowMessage
      ("In CreateObject, about to set object manage option %d\n", manage);
   #endif
   
   // Set manage option to Moderator
   theModerator->SetObjectManageOption(manage);
   
   //======================================================================
   // This block of code is the future implementation of creating objects
   // of non-special object types in general way. This will avoid adding
   // specific methods to the Moderator when we create new object type
   // through the plug-in code. This is just initial coding and needs
   // thorough testing. (LOJ: 2010.05.05)
   //======================================================================
   #ifdef __NEW_WAY_OF_CREATE_OBJECT__
   //======================================================================
   
   // Handle container or special object type creation
   if (type == "PropSetup")
      obj = (GmatBase*)theModerator->CreatePropSetup(name);
   
   // Handle Spacecraft
   else if (type == "Spacecraft" || type == "Formation")
      obj = (GmatBase*)theModerator->CreateSpacecraft(type, name, createDefault);
   
   // Handle AxisSystem
   else if (find(axisSystemList.begin(), axisSystemList.end(), type) != 
            axisSystemList.end())
      obj =(GmatBase*) theModerator->CreateAxisSystem(type, name);
   
   // Handle Burns
   else if (find(burnList.begin(), burnList.end(), type) != 
            burnList.end())
      obj = (GmatBase*)theModerator->CreateBurn(type, name, createDefault);
   
   // Handle CoordinateSystem
   else if (type == "CoordinateSystem")
      obj = (GmatBase*)theModerator->CreateCoordinateSystem(name, false, false, manage);
   
   // Handle CelestialBody
   else if (find(celestialBodyList.begin(), celestialBodyList.end(), type) != 
            celestialBodyList.end())
      obj = (GmatBase*)theModerator->CreateCelestialBody(type, name);
   
   // Handle CalculatedPoint
   else if (find(calculatedPointList.begin(), calculatedPointList.end(), type) != 
            calculatedPointList.end())
      obj =(GmatBase*) theModerator->CreateCalculatedPoint(type, name, true);
   
   // Handle Parameters
   else if (find(parameterList.begin(), parameterList.end(), type) != 
            parameterList.end())
      obj = (GmatBase*)CreateParameter(type, name, "", "");
   
   // Handle Subscribers
   else if (find(subscriberList.begin(), subscriberList.end(), type) != 
            subscriberList.end())
      obj = (GmatBase*)theModerator->CreateSubscriber(type, name);
   
   // Handle EventLocators
   else if (find(eventLocatorList.begin(), eventLocatorList.end(), type) !=
            eventLocatorList.end())
      obj = (GmatBase*)theModerator->CreateEventLocator(type, name);

   // Handle other registered creatable object types
   else if (find(allObjectTypeList.begin(), allObjectTypeList.end(), type) != 
            allObjectTypeList.end())
   {
      Gmat::ObjectType objType = GetObjectType(type);
      if (objType != Gmat::UNKNOWN_OBJECT)
         obj = theModerator->CreateOtherObject(objType, type, name);
      else
         obj = NULL;
   }
   else
   {
      obj = NULL;
   }
   
   //@note
   // Do not throw exception if obj == NULL, since caller uses return pointer
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
   
   //======================================================================
   #else
   //======================================================================
   
   if (type == "Spacecraft") 
      obj = (GmatBase*)theModerator->CreateSpacecraft(type, name, createDefault);
   
   else if (type == "Formation") 
      obj = (GmatBase*)theModerator->CreateSpacecraft(type, name);
   
   else if (type == "PropSetup") 
      obj = (GmatBase*)theModerator->CreatePropSetup(name);
   
   else if (type == "MeasurementModel")
      obj = (GmatBase*)theModerator->CreateMeasurementModel(name);

   else if (type == "TrackingData")
      obj = (GmatBase*)theModerator->CreateTrackingData(name);
   
   else if (type == "DataFile")
      obj = (GmatBase*)theModerator->CreateDataFile(type, name);
   
   else if (type == "CoordinateSystem") 
      //obj = (GmatBase*)theModerator->CreateCoordinateSystem(name, true);
      obj = (GmatBase*)theModerator->CreateCoordinateSystem(name, false, false, manage);
   
   else
   {
      #ifdef DEBUG_CREATE_CELESTIAL_BODY
      MessageInterface::ShowMessage("In CreateObject, type = %s\n", type.c_str());
      MessageInterface::ShowMessage("In CreateObject, list of celestial body types are: \n");
      for (unsigned int ii = 0; ii < celestialBodyList.size(); ii++)
         MessageInterface::ShowMessage(" ... %s\n", (celestialBodyList.at(ii)).c_str());
      #endif
      // Handle Propagator
      if (find(propagatorList.begin(), propagatorList.end(), type) != 
          propagatorList.end())
         obj = (GmatBase*)theModerator->CreatePropagator(type, name);
      
      // Handle ODEModel
      if (find(odeModelList.begin(), odeModelList.end(), type) != 
          odeModelList.end())
         obj = (GmatBase*)theModerator->CreateODEModel(type, name);
      
      // Handle AxisSystem
      else if (find(axisSystemList.begin(), axisSystemList.end(), type) != 
               axisSystemList.end())
         obj =(GmatBase*) theModerator->CreateAxisSystem(type, name);
      
      // Handle Celestial Body
      else if (find(celestialBodyList.begin(), celestialBodyList.end(), type) != 
            celestialBodyList.end())
         obj = (GmatBase*)theModerator->CreateCelestialBody(type, name);
      
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
         obj = (GmatBase*)theModerator->CreateBurn(type, name, createDefault);
      
      // Handle CalculatedPoint (Barycenter, LibrationPoint)
      // Creates default Barycenter or LibrationPoint
      else if (find(calculatedPointList.begin(), calculatedPointList.end(), type) != 
               calculatedPointList.end())
         obj =(GmatBase*) theModerator->CreateCalculatedPoint(type, name, true);
      
      // Handle DataFiles
      else if (find(dataFileList.begin(), dataFileList.end(), type) != 
               dataFileList.end())
         obj = (GmatBase*)theModerator->CreateDataFile(type, name);
      
      // Handle Functions
      else if (find(functionList.begin(), functionList.end(), type) != 
               functionList.end())
         obj = (GmatBase*)theModerator->CreateFunction(type, name, manage);
      
      // Handle Hardware (tanks, thrusters, etc.)
      else if (find(hardwareList.begin(), hardwareList.end(), type) != 
               hardwareList.end())
         obj = (GmatBase*)theModerator->CreateHardware(type, name);
      
      // Handle Measurements
      else if (find(measurementList.begin(), measurementList.end(), type) !=
               measurementList.end())
         obj = (GmatBase*)theModerator->CreateMeasurement(type, name);

      // Handle Observations
      else if (find(obtypeList.begin(), obtypeList.end(), type) !=
            obtypeList.end())
         obj = (GmatBase*)theModerator->CreateObType(type, name);

      // Handle Parameters
      else if (find(parameterList.begin(), parameterList.end(), type) != 
               parameterList.end())
         obj = (GmatBase*)CreateParameter(type, name, "", "");
      
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
      
      // Handle EventLocators
      else if (find(eventLocatorList.begin(), eventLocatorList.end(), type) !=
               eventLocatorList.end())
         obj = (GmatBase*)theModerator->CreateEventLocator(type, name);

      // Handle EphemerisFile
      else if (find(ephemFileList.begin(), ephemFileList.end(), type) != 
               ephemFileList.end())
         obj = (GmatBase*)theModerator->CreateEphemerisFile(type, name);
      
      // Handle other SpacePoints
      else if (find(spacePointList.begin(), spacePointList.end(), type) != 
               spacePointList.end())
         obj = (GmatBase*)theModerator->CreateSpacePoint(type, name);
   
      // Handle TrackingSystems
      else if (find(trackingSystemList.begin(), trackingSystemList.end(), type) !=
               trackingSystemList.end())
         obj = (GmatBase*)theModerator->CreateTrackingSystem(type, name);

      // Handle Interfaces
      else if (find(interfaceList.begin(), interfaceList.end(), type) !=
               interfaceList.end())
         obj = theModerator->CreateOtherObject(Gmat::INTERFACE, type, name);
   }
   
   //@note
   // Do not throw exception if obj == NULL, since caller uses return pointer
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
   #endif
}


//------------------------------------------------------------------------------
// void SetConfiguredObjectMap()
//------------------------------------------------------------------------------
/*
 * Sets object map in use to object map from the configuration.
 */
//------------------------------------------------------------------------------
void Interpreter::SetConfiguredObjectMap()
{
   theObjectMap = theModerator->GetConfiguredObjectMap();
   theValidator->SetObjectMap(theObjectMap);
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
      theValidator->SetSolarSystem(ss);
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
// void SetObjectMap(ObjectMap *objMap, bool forFunction)
//------------------------------------------------------------------------------
/**
 * Sets object map to be used for finding objects.
 * 
 * @param <objMap> Pointer to the object map
 * @param <forFunction> True if setting object map for function (false)
 */
//------------------------------------------------------------------------------
void Interpreter::SetObjectMap(ObjectMap *objMap, bool forFunction)
{
   #ifdef DEBUG_OBJECT_MAP
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectMap() objMap=<%p>, forFunction=%d\n", objMap,
       forFunction);
   #endif
   
   if (objMap != NULL)
   {
      if (forFunction)
      {
         #ifdef DEBUG_OBJECT_MAP
         MessageInterface::ShowMessage
            ("Interpreter::SetObjectMap() Here is the current object map <%p>, "
             "it has %d objects\n", objMap, objMap->size());
         for (std::map<std::string, GmatBase *>::iterator i = objMap->begin();
              i != objMap->end(); ++i)
         {
            MessageInterface::ShowMessage
               ("   %30s  <%p><%s>\n", i->first.c_str(), i->second,
                i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
         }
         #endif
      }
      
      theObjectMap = objMap;
      theValidator->SetObjectMap(objMap);
   }
}


//------------------------------------------------------------------------------
// ObjectMap* GetObjectMap()
//------------------------------------------------------------------------------
/**
 * @return a current object map in use.
 */
//------------------------------------------------------------------------------
ObjectMap* Interpreter::GetObjectMap()
{
   return theObjectMap;
}


//------------------------------------------------------------------------------
// void SetFunction(Function *func)
//------------------------------------------------------------------------------
/*
 * Sets Function pointer for function mode interpreting and to the Validator.
 *
 * @param  func  The Function pointer to set
 */
//------------------------------------------------------------------------------
void Interpreter::SetFunction(Function *func)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Interpreter::SetFunction() function=<%p>'%s'\n", func,
       func ? func->GetName().c_str() : "NULL");
   #endif
   
   currentFunction = func;
   theValidator->SetFunction(func);
}


//------------------------------------------------------------------------------
// Function* GetFunction()
//------------------------------------------------------------------------------
/*
 * Retrives Function pointer currently set for function mode interpreting.
 */
Function* Interpreter::GetFunction()
{
   return currentFunction;
}

//------------------------------------------------------------------------------
// void SetContinueOnError(bool flag)
//------------------------------------------------------------------------------
void Interpreter::SetContinueOnError(bool flag)
{
   continueOnError = flag;
}

//------------------------------------------------------------------------------
// bool GetContinueOnError()
//------------------------------------------------------------------------------
bool Interpreter::GetContinueOnError()
{
   return continueOnError;
}


//------------------------------------------------------------------------------
// bool IsInCommandMode()
//------------------------------------------------------------------------------
bool Interpreter::IsInCommandMode()
{
   return inCommandMode;
}


//------------------------------------------------------------------------------
// bool FoundBeginMissionSequence()
//------------------------------------------------------------------------------
bool Interpreter::FoundBeginMissionSequence()
{
   return beginMissionSeqFound;
}


//------------------------------------------------------------------------------
// bool CheckUndefinedReference(GmatBase *obj, bool writeLine)
//------------------------------------------------------------------------------
/*
 * This method checks if reference object of given object exist through
 * the Validator.
 *
 * @param  obj  input object of undefined reference object to be checked
 * @param  writeLine  flag indicating whether or not line number should be
 *                    written for the error message
 */
//------------------------------------------------------------------------------
bool Interpreter::CheckUndefinedReference(GmatBase *obj, bool writeLine)
{
   debugMsg = "In CheckUndefinedReference()";
   bool isValid = theValidator->CheckUndefinedReference(obj, continueOnError);
   
   // Handle error messages here
   if (!isValid)
   {
      StringArray errList = theValidator->GetErrorList();
      for (UnsignedInt i=0; i<errList.size(); i++)
         HandleError(InterpreterException(errList[i]), writeLine);
   }
   
   return isValid;
}


//------------------------------------------------------------------------------
// bool ValidateCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Checks the input command to make sure its wrappers are set up for it
 * correctly through the Validator, if necessary.
 *
 * @param  cmd  the command to validate
 */
//------------------------------------------------------------------------------
bool Interpreter::ValidateCommand(GmatCommand *cmd)
{
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::ValidateCommand() cmd=<%p><%s>, continueOnError=%d, "
       "inFunctionMode=%d\n", cmd, cmd->GetTypeName().c_str(), continueOnError,
       inFunctionMode);
   #endif
   
   debugMsg = "In ValidateCommand()";
   
   // Check if any Parameters need to be created
   StringArray names = cmd->GetWrapperObjectNameArray();
   
   #ifdef DEBUG_VALIDATE_COMMAND
   WriteStringArray("RefParameterNames for ", cmd->GetTypeName(), names);
   #endif
   
   // Create Parameters
   // Even in the function we still need to create automatic Parameters,
   // such sat.X in mySatX = sat.X in the assignment command, in order for Validator
   // to set wrapper reference for auto object used in the function command sequence
   // during the function initialization. But we don't want to add to function's
   // automatic store at this time. It will be added during function initialization.
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("   Calling CreateSystemParameter() for each ref. names\n");
   #endif
   for (UnsignedInt i=0; i<names.size(); i++)
   {
      CreateSystemParameter(names[i]);
   }
   
   // If in function mode, just return true,
   // ValidateCommand() is called from GmatFunction::Initialize()
   if (inFunctionMode)
   {
      #ifdef DEBUG_VALIDATE_COMMAND
      MessageInterface::ShowMessage
         ("Interpreter::ValidateCommand() in function mode, so just returning true\n");
      #endif
      return true;
   }
   
   bool isValid = theValidator->ValidateCommand(cmd, continueOnError, 1);
   
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage("   theValidator->ValidateCommand() returned %d\n", isValid);
   #endif
   
   // Handle error messages here
   if (!isValid)
   {
      StringArray errList = theValidator->GetErrorList();
      for (UnsignedInt i=0; i<errList.size(); i++)
         HandleError(InterpreterException(errList[i]));
   }
   
   #ifdef DEBUG_VALIDATE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::ValidateCommand() returning %d\n", isValid);
   #endif
   
   return isValid;
   
} // ValidateCommand()


//------------------------------------------------------------------------------
// bool ValidateSubscriber(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Creates subscriber element wrappers through Validator and sets to subscriber.
 * This method is also called from GUI subscriber setup panel to create new wrappers.
 * Currently ReportFile and XYPlot uses element wrappers.
 *
 * @param <obj> the subscriber to create wrappers
 */
//------------------------------------------------------------------------------
bool Interpreter::ValidateSubscriber(GmatBase *obj)
{
   if (obj == NULL)
      throw InterpreterException("The subscriber object to be validated is NULL");
   
   // Now continue validation
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Interpreter::ValidateSubscriber() entered, obj=<%p><%s>\n", obj,
       obj->GetName().c_str());
   #endif
   
   debugMsg = "In ValidateSubscriber()";
   
   // This method can be called from other than Interpreter, so check if
   // object is SUBSCRIBER type
   if (!obj->IsOfType(Gmat::SUBSCRIBER))
   {
      InterpreterException ex
         ("ElementWrapper for \"" + obj->GetName() + "\" of type \"" +
          obj->GetTypeName() + "\" cannot be created.");
      HandleError(ex);
      return false;
   }
   
   Subscriber *sub = (Subscriber*)obj;
   
   // We don't want to clear wrappers since Subscriber::ClearWrappers() changed to
   // also empty wrappers.  (LOJ: 2009.03.12)
   //sub->ClearWrappers();
   const StringArray wrapperNames = sub->GetWrapperObjectNameArray();
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("In ValidateSubscriber, has %d wrapper names:\n", wrapperNames.size());
   for (Integer ii=0; ii < (Integer) wrapperNames.size(); ii++)
      MessageInterface::ShowMessage("   '%s'\n", wrapperNames[ii].c_str());
   #endif
   
   for (StringArray::const_iterator i = wrapperNames.begin();
        i != wrapperNames.end(); ++i)
   {
      // Skip blank name (LOJ: 2012.08.09)
      if ((*i) == "")
         continue;
      
      try
      {
         ElementWrapper *ew = theValidator->CreateElementWrapper(*i, true);
         
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
 * it tries to find property from the owned objects.
 *
 * @param  obj    Object to find property
 * @param  chunk  String contains property
 * @param  owner  Address of new owner pointer to be returned
 * @param  id     Property ID to return (-1 if property not found)
 * @param  type   Property type to return
 *                (Gmat::UNKNOWN_PARAMETER_TYPE if property not found)
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
   #ifdef DEBUG_FIND_PROP_ID
   MessageInterface::ShowMessage
      ("Interpreter::FindPropertyID() entered, obj=<%p><%s>, chunk=<%s>\n", obj,
       (obj == NULL ? "NULL" : obj->GetName().c_str()), chunk.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(chunk);
   Integer count = parts.size();
   std::string prop = parts[count-1];
   
   #ifdef DEBUG_FIND_PROP_ID
   MessageInterface::ShowMessage("   property=<%s>\n", prop.c_str());
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
   catch (BaseException &)
   {
      #ifdef DEBUG_FIND_PROP_ID
      MessageInterface::ShowMessage("   Trying to find id from the owned object\n");
      #endif
      // Owned objects are not configurable and they are local objects
      if (FindOwnedObject(obj, prop, owner, id, type))
      {
         retval = true;
      }
      else
      {
         // Bug 2445 fix
         // Check if it is property of associated objects, such as Hardware of Spacecraft.
         // Hardware objects are configurable, but those are cloned before association.
         // So that same Hardware can be associated with multiple Spacecraft.
         if (obj->IsOfType(Gmat::SPACECRAFT))
         {
            StringArray refObjNames = obj->GetRefObjectNameArray(Gmat::HARDWARE);
            #ifdef DEBUG_FIND_PROP_ID
            WriteStringArray("Hardware objects ", obj->GetName(), refObjNames);
            #endif
            
            GmatBase *refObj = NULL;
            for (UnsignedInt i = 0; i < refObjNames.size(); i++)
            {
               refObj = FindObject(refObjNames[i]);
               if (FindPropertyID(refObj, chunk, owner, id, type))
               {
                  retval = true;
                  break;
               }
            }
         }
      }
   }
   
   #ifdef DEBUG_FIND_PROP_ID
   MessageInterface::ShowMessage
      ("Interpreter::FindPropertyID() returning id=%d, type=%d, owner=<%p><%s><%s>, "
       "retval=%d\n", id, type, *owner,
       ((*owner) == NULL ? "NULL" : (*owner)->GetTypeName().c_str()),
       ((*owner) == NULL ? "NULL" : (*owner)->GetName().c_str()), retval);
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
   return theValidator->FindObject(name, ofType);
}


//------------------------------------------------------------------------------
// bool ParseAndSetCommandName(GmatCommand *cmd, const std::string &cmdType, ...)
//------------------------------------------------------------------------------
/**
 * Parses command name from the command descriptoin, such as Propagate 'name' ...
 *
 * @param  cmd  Command pointer
 * @param  cmdType  Command type name
 * @param  desc  Original command input parameter
 * @param  newDesc  New command input parameter after command name removed
 *
 * @return true if command name not found or command name found and enclosed
 *              with single quote; false otherwise
 */
//------------------------------------------------------------------------------
bool Interpreter::ParseAndSetCommandName(GmatCommand *cmd, const std::string &cmdType,
                                         const std::string &desc, std::string &newDesc)
{
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("ParseAndSetCommandName() entered, cmd=<%s>'%s', cmdType='%s'\n      desc = <%s>"
       "\n   newDesc = <%s>\n", cmd->GetTypeName().c_str(), cmd->GetName().c_str(),
       cmdType.c_str(), desc.c_str(), newDesc.c_str());
   #endif
   
   if (desc[0] == '\'')
   {
      bool fileTypeParamFound = HasFilenameTypeParameter(cmd);
      
      #ifdef DEBUG_CREATE_COMMAND
      Integer paramCount = cmd->GetParameterCount();
      MessageInterface::ShowMessage
         ("   ==> There are %d parameters and file type parameter %s\n",
          paramCount, fileTypeParamFound ? "FOUND" : "NOT FOUND");
      MessageInterface::ShowMessage
         ("   ==> First char is a single quote, it might be a command name\n");
      #endif
      std::string::size_type index1 = desc.find('\'', 1);
      
      // if matching quote found, continue
      if (index1 != desc.npos)
      {
         // Check for more single quotes where another single quotes within the
         // command name is not allowed (e.g. 'Someone's Propagate')
         Integer numQuotes = GmatStringUtil::NumberOfOccurrences(desc, '\'');
         
         // If number of ' is greater 2, then it may be an error
//         if (numQuotes > 2)
//         {
//            // DJC: This assumption is not true; there are several other core
//            // commands (including Propagate) that allow more than 2 single
//            // quotes.  Additionally, plugin commands should not be made to
//            // suffer from this restriction.  Because of these considerations,
//            // this part of the code was reworked.
//
//            // Currently only SaveMission allows 4 single quotes including command name.
//            // SaveMission 'save mission' 'mymissionfile.txt'
//            bool error = true;
//            if (fileTypeParamFound && numQuotes == 4)
//               error = false;
//
//            if (error)
//            {
//               InterpreterException ex
//                  ("Found invalid syntax for \"" + cmdType +
//                   "\" command, single quotes within the command name is not allowed");
//               HandleError(ex);
//               return false;
//            }
//         }
//
//         StringArray parts = GmatStringUtil::SeparateBy(newDesc, "'");
//         #ifdef DEBUG_CREATE_COMMAND
//         WriteStringArray("   --->command parts", "", parts);
//         #endif
//
//         std::string cmdName = parts[0];
//         // Set command name
//         if (parts.size() == 1)
//         {
//            //@todo Until we figure out the way to get own class parameter count
//            // use 4 here which is GmatCommand parameter counter.
//            // If new parameters are added to GmatBase or GmatCommand, this number
//            // needs to be changed also.
//            // We need to add a new virtual method GetOwnParameterCount() to
//            // GmatBase later in a future.
//            if (paramCount == 4)
//            {
//               cmd->SetName(cmdName);
//               newDesc = "";
//            }
//         }
//         else if (parts.size() >= 2)
//         {
//            if (fileTypeParamFound && parts.size() == 3)
//               newDesc = parts[2];
//            else
//               newDesc = parts[1];
//            cmd->SetName(cmdName);
//         }

         std::string cmdName = "";
         
         // For now, commands with file type parameters assume that 4 quotes
         // are needed to have a command name
         if ((fileTypeParamFound == false) || (numQuotes == 4))
         {
            cmdName = desc.substr(1,index1-1);
            cmd->SetName(cmdName);
            newDesc = desc.substr(index1+1);
         }
      }
      else
      {
         InterpreterException ex
            ("Found invalid syntax for \"" + cmdType +
             "\" command, possible missing single quote for the command name");
         HandleError(ex);
         return false;
      }
   }
   
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("ParseAndSetCommandName() returning true, cmd=<%s>'%s'\n      desc = <%s>"
       "\n   newDesc = <%s>\n", cmd->GetTypeName().c_str(), cmd->GetName().c_str(),
       desc.c_str(), newDesc.c_str());
   #endif
   return true;
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
      ("Interpreter::CreateCommand() type=<%s>, inCmd=<%p>, \n   desc=<%s>\n",
       type.c_str(), inCmd, desc.c_str());
   MessageInterface::ShowMessage
      ("   inFunctionMode=%d, hasFunctionDefinition=%d\n", inFunctionMode,
       hasFunctionDefinition);
   #endif
   
   GmatCommand *cmd = NULL;
   std::string type1 = type;
   std::string desc1 = desc;
   std::string cmdStr = type + " " + desc;
   
   std::string realDesc; // Command description after name removed
   bool commandFound = false;
   bool nameParsed = true;
   
   // handle blank type
   if (type == "")
   {
      std::string::size_type index = desc.find("(");
      type1 = desc.substr(0, index);
   }
   
   if (IsCommandType(type1))
   {
      commandFound = true;
      if (type1 == "BeginMissionSequence")
         beginMissionSeqFound = true;
   }
   
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("   type1='%s', commandFound=%d\n", type1.c_str(), commandFound);
   #endif
   
   // Check for CallFunction
   if (type1[0] == '[')
   {
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage
         ("Interpreter::CreateCommand() detected [ and creating CallFunction ...\n");
      #endif

      type1 = "CallFunction";
      
      // Figure out if which CallFunction to be created.
      std::string funcName = GmatStringUtil::ParseFunctionName(desc);
      if (funcName != "")
      {
         GmatBase *func = FindObject(funcName);
         if (func != NULL && func->IsOfType("MatlabFunction"))
            type1 = "CallMatlabFunction";
         else
         {
            if (gmatFunctionsAvailable)
               type1 = "CallGmatFunction";
            else
               throw InterpreterException("The function \"" + funcName +
                     "\" is not available; if it is a GmatFunction, you may "
                     "need to enable the GmatFunction plugin "
                     "(libGmatFunction)");
         }
      }
      
      #ifdef DEBUG_CREATE_CALL_FUNCTION
      MessageInterface::ShowMessage
         ("   1 Now creating <%s> command and setting GenString to <%s>\n",
          type1.c_str(), std::string(type1 + " " + desc).c_str());
      #endif
      
      // Create CallFunction command and append to command sequence
      cmd = AppendCommand(type1, retFlag, inCmd);
      desc1 = type1 +  "=" + desc;
      if (cmd != NULL)
         cmd->SetGeneratingString(desc1);
   }
   /// @TODO: This is a work around for a call function
   /// without any return parameters.  It should be updated in
   /// the design to handle this situation.
   else if ((desc1.find("=") == desc1.npos) && (desc != "")
            && (!commandFound))
   {
      StringArray parts = theTextParser.SeparateSpaces(desc1);
      
      #ifdef DEBUG_CREATE_CALL_FUNCTION
      WriteStringArray("Calling IsObjectType()", "", parts);
      #endif
      
      if (IsObjectType(parts[0]))
      {
         InterpreterException ex("Found invalid command \"" + type1 + "\"");
         HandleError(ex);
      }
      else if (!GmatStringUtil::IsValidName(type1 + desc, true))
      {
         InterpreterException ex
            ("Found invalid function name \"" + type1 + desc + "\"");
         HandleError(ex);
      }
      else
      {
         type1 = "CallFunction";
         
         std::string funcName = GmatStringUtil::ParseFunctionName(desc);
         if (funcName != "")
         {
            GmatBase *func = FindObject(funcName);
            if (func != NULL && func->IsOfType("MatlabFunction"))
               type1 = "CallMatlabFunction";
            else
            {
               if (gmatFunctionsAvailable)
                  type1 = "CallGmatFunction";
               else
                  throw InterpreterException("The function \"" + funcName +
                        "\" is not available; if it is a GmatFunction, you may "
                        "need to enable the GmatFunction plugin "
                        "(libGmatFunction)");
            }
         }
         
         #ifdef DEBUG_CREATE_CALL_FUNCTION
         MessageInterface::ShowMessage
            ("   2 Now creating <%s> command and setting GenString to <%s>\n",
             type1.c_str(), std::string(type1 + " " + desc).c_str());
         #endif
         
         // Create command and append to command sequence
         cmd = AppendCommand(type1, retFlag, inCmd);
         desc1 = "[] =" + type1 + desc;
         if (cmd != NULL)
            cmd->SetGeneratingString(desc1);
      }
   }
   else
   {
      if (type1 == "CallFunction")
      {
         std::string funcName = GmatStringUtil::ParseFunctionName(desc);
         
         #ifdef DEBUG_CREATE_CALL_FUNCTION
         MessageInterface::ShowMessage("   funcName = '%s'\n", funcName.c_str());
         #endif
         
         if (funcName != "")
         {
            GmatBase *funcPtr = FindObject(funcName);
            
            #ifdef DEBUG_CREATE_CALL_FUNCTION
            MessageInterface::ShowMessage("   funcPtr=<%p>\n", funcPtr);
            MessageInterface::ShowMessage
               ("   matlabFunctionNames.size()=%d\n", matlabFunctionNames.size());
            for (UnsignedInt ii = 0; ii < matlabFunctionNames.size(); ii++)
               MessageInterface::ShowMessage
                  ("      '%s'\n", matlabFunctionNames[ii].c_str());
            #endif
            
            // If function name found in matlabFunctionNames, create
            // CallMatlabFunction (LOJ: Bug 1967 fix)
            if (find(matlabFunctionNames.begin(), matlabFunctionNames.end(),
                     funcName) != matlabFunctionNames.end())
            {
               type1 = "CallMatlabFunction";
            }
            else
            {
               if (funcPtr != NULL && funcPtr->IsOfType("MatlabFunction"))
                  type1 = "CallMatlabFunction";
               else
               {
                  if (gmatFunctionsAvailable)
                     type1 = "CallGmatFunction";
                  else
                     throw InterpreterException("The function \"" + funcName +
                           "\" is not available; if it is a GmatFunction, you "
                           "may need to enable the GmatFunction plugin "
                           "(libGmatFunction)");
               }
            }
         }
      }
      
      // How do we detect MatlabFunction inside a GmatFunction?
      if (desc.find("MatlabFunction") != desc.npos)
      {
         StringArray parts = GmatStringUtil::SeparateBy(desc, " ");
         if (parts.size() == 2)
         {
            #ifdef DEBUG_CREATE_CALL_FUNCTION
            MessageInterface::ShowMessage
               ("   Adding '%s' to matlabFunctionNames\n", parts[1].c_str());
            #endif
            matlabFunctionNames.push_back(parts[1]);
         }
      }
      
      #if defined (DEBUG_CREATE_COMMAND) || defined (DEBUG_CREATE_CALL_FUNCTION)      
      MessageInterface::ShowMessage("   3 Now creating <%s> command\n", type1.c_str());
      #endif
      
      // Create a command and append to command sequence
      cmd = AppendCommand(type1, retFlag, inCmd);
      realDesc = desc;
      
      // If command is not call function, parse command name
      if (cmd != NULL && !cmd->IsOfType("CallFunction"))
         nameParsed = ParseAndSetCommandName(cmd, type1, desc, realDesc);
      
      #if defined (DEBUG_CREATE_COMMAND) || defined (DEBUG_CREATE_CALL_FUNCTION)      
      MessageInterface::ShowMessage
         ("   Setting GenString to <%s>\n", std::string(type1 + " " + realDesc).c_str());
      #endif
      cmd->SetGeneratingString(type1 + " " + realDesc);
   }
   
   if (cmd == NULL)
   {
      retFlag = false;
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage
         ("CreateCommand() returning NULL for '%s', retFlag=%d\n", type1.c_str(),
          retFlag);
      #endif
      return NULL;
   }
   
   if (!nameParsed)
   {
      retFlag = false;
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage
         ("CreateCommand() leaving creating '%s', command name was not parsed correctly, "
          "cmd=<%p>, retFlag=%d\n", type1.c_str(), cmd, retFlag);
      #endif
      
      // Return cmd since command already created
      return cmd;
   }
   
   #ifdef DEBUG_CREATE_COMMAND
   if (inCmd == NULL)
      MessageInterface::ShowMessage
         ("   => <%s>'%s' created.\n", cmd->GetTypeName().c_str(), cmd->GetName().c_str());
   else
      MessageInterface::ShowMessage
         ("   => <%s>'%s' created and appended to '%s'.\n",
          cmd->GetTypeName().c_str(), cmd->GetName().c_str(), inCmd->GetTypeName().c_str());
   MessageInterface::ShowMessage
      ("     desc     = <%s>\n     desc1    = <%s>\n     realDesc = <%s>\n",
       desc.c_str(), desc1.c_str(), realDesc.c_str());
   #endif
   
   // Now assemble command
   try
   {
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage
         ("   => Now calling %s->InterpretAction()\n", type1.c_str());
      #endif
      
      // Set current function to command 
      cmd->SetCurrentFunction(currentFunction);
      
      // if command has its own InterpretAction(), just return cmd
      if (cmd->InterpretAction())
      {
         // if command is Assignment, check if GmatFunction needs to be created
         if (type1 == "GMAT" && ((Assignment*)cmd)->GetMathTree() != NULL)
            HandleMathTree(cmd);
         
         #ifdef DEBUG_CREATE_COMMAND
         MessageInterface::ShowMessage("   => Now calling ValidateCommand()\n");
         #endif
         retFlag  = ValidateCommand(cmd);
         
         // For Solver commands make sure Solver type is correct
         if (cmd->IsOfType("Optimize") || cmd->IsOfType("Target") ||
             cmd->IsOfType("Achieve")  || cmd->IsOfType("Minimize") ||
             cmd->IsOfType("NonlinearConstraint") )
         {
            std::string expSolverType = "DifferentialCorrector";
            if (cmd->IsOfType("Optimize") || cmd->IsOfType("Minimize") ||
                cmd->IsOfType("NonlinearConstraint"))
               expSolverType = "Optimizer";

            std::string solverName;
            try
            {
               solverName = cmd->GetRefObjectName(Gmat::SOLVER);
            }
            catch (BaseException &)
            {
               if (solverName == "")
                  solverName = cmd->GetStringParameter("SolverName");

               if (solverName == "")
                  throw;
            }

            GmatBase *obj = FindObject(solverName);
            
            #ifdef DEBUG_CREATE_COMMAND
            MessageInterface::ShowMessage
               ("   solver=<%p><%s>'%s'\n", obj,
                obj ? obj->GetTypeName().c_str() : "NULL",
                obj ? obj->GetName().c_str() : "NULL");
            #endif
            
            // If solver object can be found, check solver type
            // (There will be no solver available inside a function until execution)
            if (obj != NULL)
            {
               bool wrongSolver = false;
               
               if (!obj->IsOfType(expSolverType))
                  wrongSolver = true;
               
               if (wrongSolver)
               {
                  InterpreterException ex
                     ("The Solver \"" + solverName + "\" is not a(n) " + expSolverType);
                  HandleError(ex);
                  retFlag = false;
               }
            }
         }
         
         #ifdef DEBUG_CREATE_COMMAND
         MessageInterface::ShowMessage("   ===> %s has own InterpretAction()\n", type1.c_str());
         MessageInterface::ShowMessage
            ("CreateCommand() leaving creating '%s', cmd=<%p>, retFlag=%d\n", type1.c_str(),
             cmd, retFlag);
         #endif
         
         return cmd;
      }
      else
      {
         #ifdef DEBUG_CREATE_COMMAND
         MessageInterface::ShowMessage
            ("   ===> %s does not have own InterpretAction()\n", type1.c_str());
         #endif
      }
   }
   catch (BaseException &e)
   {
      HandleError(e);
      retFlag = false;
      
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage
         ("   => Caught exception: '%s'\n", e.GetFullMessage().c_str());
      MessageInterface::ShowMessage
         ("CreateCommand() leaving creating '%s', cmd=<%p>, retFlag=%d\n", type1.c_str(),
          cmd, retFlag);
      #endif
      
      // Return cmd since command already created
      return cmd;
   }
   
   
   // Assemble commands those don't have InterpretAction()
   if (realDesc != "")
   {
      bool retval3 = true;
      bool retval1  = AssembleCommand(cmd, realDesc);
      
      if (retval1)
         retval3 = ValidateCommand(cmd);
      else
      {
         if (!ignoreError)
         {
            InterpreterException ex("Failed to parse " + cmdStr);
            HandleError(ex);
         }
      }
      
      retFlag = retval1 && retval3;
      
   }
   else
   {
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage("   There is no command descriptions to assemble\n");
      #endif
   }
   
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("CreateCommand() leaving creating '%s', cmd=<%p>, retFlag=%d\n", type1.c_str(),
       cmd, retFlag);
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
//GmatCommand* AppendCommand(const std::string &type, bool &retFlag,
//                           GmatCommand *inCmd)
//------------------------------------------------------------------------------
GmatCommand* Interpreter::AppendCommand(const std::string &type, bool &retFlag,
                                        GmatCommand *inCmd)
{
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::AppendCommand() entered, type=<%s>, inCmd=<%p>\n", type.c_str(), inCmd);
   #endif
   
   GmatCommand *cmd = NULL;
   
   if (inCmd == NULL)
   {
      cmd = theModerator->AppendCommand(type, "", retFlag);
      
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage
         ("===> Appending command <%s> to the last command\n",
          cmd->GetTypeName().c_str());
      #endif
   }
   else
   {
      cmd = theModerator->CreateCommand(type, "", retFlag);
      inCmd->Append(cmd);
      
      #ifdef DEBUG_CREATE_COMMAND
      MessageInterface::ShowMessage
         ("===> Appending command <%s> to <%s>\n", cmd->GetTypeName().c_str(),
          inCmd->GetTypeName().c_str());
      #endif
   }
   
   #ifdef DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::AppendCommand() returning <%p><%s>'%s', retFlag=%d\n", cmd,
       cmd->GetTypeName().c_str(), cmd->GetName().c_str(), retFlag);
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
//bool AssembleCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleCommand(GmatCommand *cmd, const std::string &desc)
{
   ignoreError = false;
   bool retval = false;
   std::string type = cmd->GetTypeName();
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::AssembleCommand() cmd=<%s>'%s'\n   desc=<%s>\n",
       type.c_str(), cmd->GetName().c_str(), desc.c_str());
   #endif
   
   if (cmd->IsOfType("For"))
      retval = AssembleForCommand(cmd, desc);
   else if (cmd->IsOfType("CallFunction"))
      retval = AssembleCallFunctionCommand(cmd, desc);
   else if (cmd->IsOfType("ConditionalBranch"))
      retval = AssembleConditionalCommand(cmd, desc);
   else
      retval = AssembleGeneralCommand(cmd, desc);
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("AssembleCommand() leaving assembling %s, retval=%d\n",
       type.c_str(), retval);
   #endif
   
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
   
   debugMsg = "In AssembleCallFunctionCommand()";
   std::string cmdTypeName = cmd->GetTypeName();
   bool retval = true;
   
   // Output
   std::string::size_type index1 = 0;
   std::string lhs;
   StringArray outArray;
   
   // Get the command name, if there is one
   std::string newDesc = desc;
   if (!ParseAndSetCommandName(cmd, cmdTypeName, desc, newDesc))
   {
      #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
         MessageInterface::ShowMessage("The command is not named\n");
      #endif
   }
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
      else
         MessageInterface::ShowMessage("The command is named \"%s\"\n",
               cmd->GetName().c_str());
   #endif

   // get output arguments if there was an equal sign
   if (GmatStringUtil::IsThereEqualSign(newDesc))
   {
      index1 = newDesc.find("=");
      lhs = newDesc.substr(0, index1);
      outArray = theTextParser.SeparateBrackets(lhs, "[]", " ,", true);
      index1 = index1 + 1;
   }
   
   // Function Name, Input
   StringArray inArray;
   std::string funcName;
   std::string::size_type index2 = newDesc.find("(", index1);
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage
      ("   Starting index=%u, open parenthesis index=%u\n", index1, index2);
   #endif
   
   if (index2 == newDesc.npos)
   {      
      funcName = newDesc.substr(index1);
   }
   else
   {
      funcName = newDesc.substr(index1, index2-index1);
      std::string rhs = newDesc.substr(index2);
      rhs = GmatStringUtil::RemoveOuterString(rhs, "(", ")");
      
      #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
      MessageInterface::ShowMessage("   rhs=\"%s\"\n", rhs.c_str());
      #endif
      
      // check if single quote found
      inArray = GmatStringUtil::SeparateByComma(rhs);
      
      #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
      MessageInterface::ShowMessage("   inArray.size()=%d\n", inArray.size());
      #endif
   }
   
   funcName = GmatStringUtil::Trim(funcName);
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage
      ("   Checking function name '%s'\n", funcName.c_str());
   #endif
   
   // Check for blank name
   if (funcName == "")
   {
      InterpreterException ex("Found blank function name");
      HandleError(ex);
      ignoreError = true;
      return false;
   }
   
   // Check for valid name
   if (!GmatStringUtil::IsValidName(funcName))
   {
      InterpreterException ex("Found invalid function name \"" + funcName + "\"");
      HandleError(ex);
      ignoreError = true;
      return false;
   }
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage("   Setting funcName '%s'\n", funcName.c_str());
   #endif
   
   // Special case for MatlabFunction
   // If in functin mode and function name is found from tempObjectNames,
   // add an extension
   std::string newFuncName = funcName;
   
   if (inFunctionMode)
   {
      if (find(tempObjectNames.begin(), tempObjectNames.end(), funcName) !=
          tempObjectNames.end())
      {
         GmatGlobal *global = GmatGlobal::Instance();
         newFuncName = funcName + global->GetMatlabFuncNameExt();
         
         #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
         MessageInterface::ShowMessage
            ("   '%s' found in tempObjectNames, so setting '%s' as function "
             "name\n", funcName.c_str(), newFuncName.c_str());
         #endif
      }
   }
   
   // Set function name to CallFunction
   retval = cmd->SetStringParameter("FunctionName", newFuncName);
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage("   Setting input\n");
   WriteStringArray("CallFunction Input", "", inArray);
   #endif
   
   //-----------------------------------------------------------------
   // Set input to CallFunction
   //-----------------------------------------------------------------
   bool validInput = false;
   Real rval;
   std::string errmsg = "disallowed";
   
   if (inArray.size() == 0) //if no inputs, set validInput to true
      validInput = true;
   
   for (UnsignedInt i=0; i<inArray.size(); i++)
   {            
      std::string input = inArray[i];
      
      if (GmatStringUtil::IsEnclosedWith(input, "'"))
         retval = cmd->SetStringParameter("AddInput", input);
      else
      {
         // Get variable names from input
         // arr33(var1, var2) should return arr33, var1, var2
         StringArray varNames = GmatStringUtil::GetVarNames(input);
         
         #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
         MessageInterface::ShowMessage("   varNames.size()=%d\n", varNames.size());
         for (unsigned int i = 0; i < varNames.size(); i++)
            MessageInterface::ShowMessage("   varNames[%d] = '%s'\n", i, varNames[i].c_str());
         #endif
         
         // Currently array element cannot be passed to CallMatlabFunction and
         // array element with variable index cannot be passed to CallGmatFunction.
         // If array element index is other than number, set input to "" so thatn
         // CallFunction will return false. Array element with numeric index will
         // be checked below.
         if (varNames.size() > 1)
            input = "";
         
         #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
         MessageInterface::ShowMessage
            ("   Setting <%s> as input to %s\n", input.c_str(), cmdTypeName.c_str());
         #endif
         
         retval = cmd->SetStringParameter("AddInput", input);
         if (!retval)
            validInput = false;
      }
      
      // If no error from CallFunction command, do more checking on input
      if (retval)
      {
         // Check for valid input parameter
         validInput = false;
         
         // Check for number before object property.
         // This fixes Bug1903 (Failed to pass number and literal to a function)
         
         // String literal
         if (GmatStringUtil::IsEnclosedWith(inArray[i], "'"))
         {
            if (cmdTypeName == "CallGmatFunction")
               validInput = true;
         }
         // Number
         else if (GmatStringUtil::ToReal(inArray[i], rval))
         {
            if (cmdTypeName == "CallGmatFunction")
               validInput = true;
         }
         // Parameter or object property
         else if (inArray[i].find('.') != std::string::npos)
         {
            // if input parameter is a system Parameter then create
            if (IsParameterType(inArray[i]))
            {
               Parameter *param = CreateSystemParameter(inArray[i]);
               if (param != NULL)
               {
                  #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
                  MessageInterface::ShowMessage
                     ("   The parameter <%s> is created\n", inArray[i].c_str());
                  #endif
                  validInput = true;
               }
            }
            else
            {
               if (cmdTypeName == "CallGmatFunction")
                  validInput = true;
            }
         }
         // Whole object
         else
         {
            // Only allow Variable, whole Array, String, and Parameter as
            // CallMatlabFunction input parameter.
            // (For GMT-3316/3324 fix - LOJ: 2012.12.26)
            GmatBase *obj = FindObject(inArray[i]);
            if (!obj) errmsg = "nonexistent";
            if (obj != NULL)
            {
               validInput = true;
               
               if (cmdTypeName == "CallMatlabFunction")
               {
                  if (obj->IsOfType("UserParameter"))
                  {
                     #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
                     MessageInterface::ShowMessage("   <%s> is an UserParameter\n", input.c_str());
                     #endif
                  }
                  else
                     validInput = false;
               }
            }
         }
      }
      
      #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
      MessageInterface::ShowMessage
         ("   <%s> is %svalid input\n", inArray[i].c_str(), validInput ? "" : "not ");
      #endif
      
      // If in function mode, ignore invalid parameter
      if (retval && inFunctionMode)
         validInput = true;
      
      // If not in function mode, throw exception if invalid inputparameter
      if (!retval || !validInput)
      {
         InterpreterException ex
            ("\"" + inArray[i] + "\" is " + errmsg + " " + cmdTypeName + " Input Parameter. "
             "The allowed input values are : [Variable, Array, Array Element, String, Parameter]");
         HandleError(ex);
         ignoreError = true;
         
         #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
         MessageInterface::ShowMessage
            ("Interpreter::AssembleCallFunctionCommand() returning false, "
             "retval=%d, validInput=%d\n", retval, validInput);
         #endif
         return false;
      }
   }   
   
   //-----------------------------------------------------------------
   // Set output to CallFunction
   //-----------------------------------------------------------------
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage("   Setting output\n");
   WriteStringArray("CallFunction Output", "", outArray);
   #endif
   
   bool validOutput = false;
   errmsg = "disallowed";
   
   for (UnsignedInt i=0; i<outArray.size(); i++)
   {
      retval = cmd->SetStringParameter("AddOutput", outArray[i]);
      if (retval && cmd->GetTypeName() == "CallGmatFunction")
      {
         validOutput = true;
      }
      // Validate output for CallMatlabFunction
      // Currently whole Array, Variable, String, and Parameter are allowed
      else if (retval && cmd->GetTypeName() == "CallMatlabFunction")
      {
         // If Parameter or object field, check further
         if (outArray[i].find('.') != std::string::npos)
         {
            // Currently CallMatlabFunction cannot handle object field
            // If output parameter is a system Parameter then create
            if (IsParameterType(outArray[i]))
            {
               Parameter *param = CreateSystemParameter(outArray[i]);
               if (param != NULL)
               {
                  #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
                  MessageInterface::ShowMessage
                     ("   The parameter <%s> is created\n", inArray[i].c_str());
                  #endif
                  validOutput = true;
               }
               else
               {
                  if (cmdTypeName == "CallGmatFunction")
                     validOutput = true;
               }
            }
         }
         else
         {
            // Only allow Variable, whole Array, String, and Parameter as
            // CallMatlabFunction output parameter
            // (For GMT-3316/3324 fix - LOJ: 2012.12.26)
            GmatBase *obj = FindObject(outArray[i]);
            if (!obj) errmsg = "nonexistent";
            if (obj != NULL)
            {
               validOutput = true;
               
               if (obj->IsOfType("UserParameter"))
               {
                  #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
                  MessageInterface::ShowMessage
                     ("   <%s> is not an UserParameter\n", outArray[i].c_str());
                  #endif
               }
               else
                  validOutput = false;
            }
         }
      }
      
      // If not in function mode, throw exception if invalid output parameter
      if (!retval || !validOutput)
      {
         InterpreterException ex
            ("\"" + outArray[i] + "\" is " + errmsg + " " + cmdTypeName + " Output Parameter. "
             "The allowed output values are : [Variable, Array, Array Element, String, Parameter]");
         HandleError(ex);
         ignoreError = true;
         
         #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
         MessageInterface::ShowMessage
            ("Interpreter::AssembleCallFunctionCommand() returning false, "
             "retval=%d, validInput=%d, validOuput=%d\n", retval, validInput, validOutput);
         #endif
         return false;
      }
   }
   
   // if in function mode, just return retval
   if (inFunctionMode)
   {
      #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
      MessageInterface::ShowMessage
         ("Interpreter::AssembleCallFunctionCommand() returning %d, it's in "
          "function mode\n", retval);
      #endif
      return retval;
   }
   
   // See if Function is MatlabFunction since all MatlabFunctions are created
   // before mission sequence, if not, create as GmatFunction.
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
   #ifdef DEBUG_ASSEMBLE_CONDITION
   MessageInterface::ShowMessage
      ("Interpreter::AssembleConditionalCommand() cmd=<%p>'%s', desc='%s'\n",
       cmd, cmd->GetTypeName().c_str(), desc.c_str());
   #endif
   
   debugMsg = "In AssembleConditionalCommand()";
   bool retval = true;
   std::string type = cmd->GetTypeName();
   std::string opStr = "~<=>&|";
   
   // conditional commands, for compatibility with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc))
   {
      std::string msg = 
         "A conditional command is not allowed to contain brackets, braces, or "
         "parentheses (except to indicate an array element)";
      InterpreterException ex(msg);
      HandleError(ex);
      ignoreError = true;
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
         ignoreError = true;
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
   
   #ifdef DEBUG_ASSEMBLE_CONDITION
   WriteStringArray("After parsing conditions()", "", parts);
   #endif
   
   Integer count = parts.size();
   for (Integer ii = 0; ii < count; ii++)
   {
      if (GmatStringUtil::IsBlank(parts.at(ii)))
      {
         InterpreterException ex("Missing field or operator in command");
         HandleError(ex);
         ignoreError = true;
         return false;
      }
      std::string strUpper = GmatStringUtil::ToUpper(parts.at(ii));         
      if (strUpper.find(" OR ") != strUpper.npos)
      {
         InterpreterException ex("\"OR\" is not a valid relational operator");
         HandleError(ex);
         ignoreError = true;
         return false;
      }
      if (strUpper.find(" AND ") != strUpper.npos)
      {
         InterpreterException ex("\"AND\" is not a valid relational operator");
         HandleError(ex);
         ignoreError = true;
         return false;
      }
   }
   
   // assuming there is no boolean argument
   if (count < 3 || ((count-3)%4) != 0)
   {
      InterpreterException ex("The Command has an invalid number of conditions");
      HandleError(ex);
      ignoreError = true;
      return false;
   }
   
   // Added try/catch block so that function name can be added to the error message
   try
   {
      ConditionalBranch *cb = (ConditionalBranch*)cmd;
      
      for (int i=0; i<count; i+=4)
      {
         #ifdef DEBUG_ASSEMBLE_CONDITION
         MessageInterface::ShowMessage
            ("   lhs:<%s>, op:<%s>, rhs:<%s>\n", parts[i].c_str(), parts[i+1].c_str(),
             parts[i+2].c_str());
         #endif
         
         // Try to create a parameter first if system parameter
         std::string type, ownerName, depObj;
         GmatStringUtil::ParseParameter(parts[i], type, ownerName, depObj);
         #ifdef DEBUG_ASSEMBLE_CONDITION // ---------------------------- debug ----
         MessageInterface::ShowMessage
            ("   lhs: type = %s, ownerName = %s, depObj = %s\n", 
             type.c_str(), ownerName.c_str(), depObj.c_str());
         #endif // ------------------------------------------------- end debug ----
         
         // Create Parameter if not in function, since Parameters are automatically
         // created in ValidateCommand
         if (!inFunctionMode)
         {
            if (theModerator->IsParameter(type))
               CreateParameter(type, parts[i], ownerName, depObj);
         }
         
         GmatStringUtil::ParseParameter(parts[i+2], type, ownerName, depObj);
         #ifdef DEBUG_ASSEMBLE_CONDITION // ---------------------------- debug ----
         MessageInterface::ShowMessage
            ("   rhs: type = %s, ownerName = %s, depObj = %s\n", 
             type.c_str(), ownerName.c_str(), depObj.c_str());
         #endif // ------------------------------------------------- end debug ----
         
         // Create Parameter if not in function, since Parameters are automatically
         // created in ValidateCommand
         if (!inFunctionMode)
         {
            if (theModerator->IsParameter(type))
               CreateParameter(type, parts[i+2], ownerName, depObj);
         }
         
         cb->SetCondition(parts[i], parts[i+1], parts[i+2]);
         
         if (count > i+3)
         {
            #ifdef DEBUG_ASSEMBLE_CONDITION
            MessageInterface::ShowMessage("   logOp=<%s>\n", parts[i+3].c_str());
            #endif
            
            cb->SetConditionOperator(parts[i+3]);
         }
      }
   }
   catch (BaseException &e)
   {
      InterpreterException ex(e.GetFullMessage());
      HandleError(ex);
      ignoreError = true;
      return false;
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
   
   debugMsg = "In AssembleForCommand()";
   
   // For loop commands, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc))
   {
      std::string msg = 
         "A For command is not allowed to contain brackets, braces, or "
         "parentheses (except to indicate an array element)";
      InterpreterException ex(msg);
      HandleError(ex);
      ignoreError = true;
      return false;
   }
   
   bool retval = true;
   std::string::size_type equalSign = desc.find("=");
   
   if (equalSign == desc.npos)
   {
      InterpreterException ex("Cannot find equal sign (=) for For loop control");
      HandleError(ex);
      ignoreError = true;
      return false;
   }
   
   std::string index = desc.substr(0, equalSign);
   index = GmatStringUtil::Trim(index);
   
   std::string substr = desc.substr(equalSign+1);
   if (substr.find(':') == substr.npos)
   {
      InterpreterException ex("Missing colon (:) for For loop control");
      HandleError(ex);
      ignoreError = true;
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
      ignoreError = true;
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
      ignoreError = true;
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
/**
 * @todo: GMAT by design accepts commands of the form
 *
 *    CommandName <ListOfParameters>
 *
 * Commands that follow this process are required to accept the listed
 * parameters, one by one, in order, and then store them for initialization or
 * process them when they are passed in to the command.
 *
 * The implementation provided here prevents this design from working, because
 * the code here is NOT GENERIC, and requires knowledge before the fact of every
 * command that GMAT will contain.  That means not just that the code here
 * seriously violates the design, but also that ANY plugin containing a new
 * command is required to implement parsing for the command.
 *
 * This code needs to be fixed to match the design so that individual commands
 * that conform to the list of arguments protocol can be added to GMAT without
 * forcing the programmer to either hack base code or implement their own
 * parsing.
 */
bool Interpreter::AssembleGeneralCommand(GmatCommand *cmd,
                                         const std::string &desc)
{
   bool retval = true;
   std::string type = cmd->GetTypeName();
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("AssembleGeneralCommand() cmd=<%s>'%s', desc=<%s>\n", cmd->GetTypeName().c_str(),
       cmd->GetName().c_str(), desc.c_str());
   #endif

   // Target, Optimize has own InterpretAction() in the parent SolverBranchCommand
   // so those are removed (LOJ: 2012.03.07)
   if (type == "Report" || type == "BeginFiniteBurn" || type == "EndFiniteBurn")
   {
      //if (type == "Target")
      //   retval = AssembleTargetCommand(cmd, desc);
      //else if (type == "Optimize")
      //   retval = AssembleOptimizeCommand(cmd, desc);
      if (type == "Report")
         retval = AssembleReportCommand(cmd, desc);
      else
         retval = AssembleFiniteBurnCommand(cmd, desc);
   }
   else if (type == "Create")
      retval = AssembleCreateCommand(cmd, desc);
   else if (type == "Save" || type == "Global")
      retval = SetCommandRefObjects(cmd, desc);
   else
      retval = false;
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("AssembleGeneralCommand() leaving assembling %s, retval=%d\n",
       type.c_str(), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool AssembleTargetCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
//@note We need to update this if we decided to use this one.
// See SolverBranchCommand::InterpretAction()
bool Interpreter::AssembleTargetCommand(GmatCommand *cmd, const std::string &desc)
{
   #ifdef DEBUG_ASSEMBLE_SOLVER_COMMAND
   MessageInterface::ShowMessage
      ("AssembleTargetCommand() entered, cmd=<%p><%s>'%s', desc=<%s>\n",
       cmd, cmd->GetTypeName().c_str(), cmd->GetName().c_str(), desc.c_str());
   #endif
   debugMsg = "In AssembleTargetCommand()";
   
   // This command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc, false))
   {
      std::string msg = 
         "The Target command is not allowed to contain brackets, braces, or "
         "parentheses";
      InterpreterException ex(msg);
      HandleError(ex);
      ignoreError = true;
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
      else
      {
         // Check if Solver type is a DifferentialCorrector
         if (!obj->IsOfType("DifferentialCorrector"))
         {
            InterpreterException ex
               ("The Solver \"" + parts[0] + "\" is not a DifferentialCorrector");
            HandleError(ex);
            retval = false;
         }
      }
   }
   
   #ifdef DEBUG_ASSEMBLE_SOLVER_COMMAND
   MessageInterface::ShowMessage
      ("AssembleTargetCommand() returning %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// bool AssembleOptimizeCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
//@note We need to update this if we decided to use this one
// See SolverBranchCommand::InterpretAction()
bool Interpreter::AssembleOptimizeCommand(GmatCommand *cmd, const std::string &desc)
{
   #ifdef DEBUG_ASSEMBLE_SOLVER_COMMAND
   MessageInterface::ShowMessage
      ("AssembleOptimizeCommand() entered, cmd=<%p><%s>'%s', desc=<%s>\n",
       cmd, cmd->GetTypeName().c_str(), cmd->GetName().c_str(), desc.c_str());
   #endif
   debugMsg = "In AssembleOptimizeCommand()";
   
   // This command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(desc, false))
   {
      std::string msg = 
         "The Optimize command is not allowed to contain brackets, braces, or "
         "parentheses";
      InterpreterException ex(msg);
      HandleError(ex);
      ignoreError = true;
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
      else
      {
         // Check if Solver type is an Optimizer
         if (!obj->IsOfType("Optimizer"))
         {
            InterpreterException ex
               ("The Solver \"" + parts[0] + "\" is not an Optimizer");
            HandleError(ex);
            retval = false;
         }
      }
   }
   
   #ifdef DEBUG_ASSEMBLE_SOLVER_COMMAND
   MessageInterface::ShowMessage
      ("AssembleOptimizeCommand() returning %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool AssembleFiniteBurnCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleFiniteBurnCommand(GmatCommand *cmd, const std::string &desc)
{
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage("Begin/EndFiniteBurn being processed ...\n");
   #endif
   
   bool retval = true;
   debugMsg = "In AssembleFiniteBurnCommand()";
   
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
   WriteStringArray(type, "", parts);
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
      WriteStringArray(type, "", subParts);
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
   
   debugMsg = "In AssembleReportCommand()";
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
   WriteStringArray("Parsing Report", "", parts);
   #endif
   
   // checking items to report
   if (count < 2)
   {
      InterpreterException ex ("There are no ReportFile or items to Report");
      HandleError(ex);
      return false;
   }
   
   // Set ReportFile name
   cmd->SetStringParameter("ReportFile", parts[0]);
   
   // Set reporting Parameter names
   for (int i=1; i<count; i++)
      cmd->SetStringParameter("Add", parts[i]);
   
   GmatBase *obj = NULL;
   
   // See if we can set ReportFile pointer
   // We can skip checking for configured object if in Function mode
   if (!inFunctionMode)
   {
      obj = FindObject(parts[0]);
      
      if (obj == NULL)
      {
         InterpreterException ex
            ("Cannot find the ReportFile \"" + parts[0] + "\"");
         HandleError(ex);
         return false;
      }
      
      // Set ReportFile pointer
      cmd->SetRefObject(obj, Gmat::SUBSCRIBER, parts[0], 0);
   }
   
   // Create Parameters to report
   for (int i=1; i<count; i++)
   {
      obj = (GmatBase*)CreateSystemParameter(parts[i]);
      
      if (!inFunctionMode)
      {
         if (obj != NULL)
         {
            // Check if it is reportable Parameter
            if (((Parameter*)obj)->IsReportable())
               cmd->SetRefObject(obj, Gmat::PARAMETER, parts[i], 0);
            else
            {
               InterpreterException ex
                  ("\"" + parts[i] + "\" is not a reportable Parameter");
               HandleError(ex);
               retval = false;
            }
         }
         else
         {
            InterpreterException ex
               ("Nonexistent or disallowed Report Variable: \"" + parts[i] +
                "\";\nCurrently object fields are not allowed to report. "
                "It will not be added to Report");
            HandleError(ex);
            retval = false;
         }
      }
   }
   
   #ifdef DEBUG_ASSEMBLE_REPORT_COMMAND 
   MessageInterface::ShowMessage("AssembleReportCommand() returning %d\n", retval);
   #endif
   
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
   
   debugMsg = "In AssembleCreateCommand()";
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
   
   //-----------------------------------------------------------------
   // check if comma is allowed in Create command (loj: 2008.08.29)
   //-----------------------------------------------------------------
   #ifdef __DISALLOW_COMMA_IN_CREATE__
   // check for comma, if comman is not allowed in Create command
   if (objNameStr.find(",") != objNameStr.npos)
   {
      InterpreterException ex
         ("Comma is not allowed in " + cmd->GetTypeName() + " command");
      HandleError(ex);
      return false;
   }
   StringArray objNames = GmatStringUtil::SeparateBy(objNameStr, " ", true);
   #else
   StringArray objNames = GmatStringUtil::SeparateBy(objNameStr, ", ", true);
   #endif
   
   
   #ifdef DEBUG_ASSEMBLE_CREATE
   WriteStringArray("Create object names", "", objNames);
   #endif
   
   if (objNames.size() == 0)
   {
      InterpreterException ex
         ("Missing object name found in " + cmd->GetTypeName() + " command");
      HandleError(ex);
      return false;
   }
   
   std::string objTypeStrToUse = objTypeStr;
   // Special case for Propagator and OpenGLPlot
   if (objTypeStr == "Propagator")
      objTypeStrToUse = "PropSetup";
   else if (objTypeStr == "OpenGLPlot")
      objTypeStrToUse = "OrbitView";
   
   try
   {
      // if object is MatlabFunction make sure we add .m extension to avoid
      // automatically creating GmatFunction in the Sandbox::HandleGmatFunction()
      cmd->SetStringParameter("ObjectType", objTypeStrToUse);
      for (UnsignedInt i=0; i<objNames.size(); i++)
         cmd->SetStringParameter("ObjectNames", objNames[i]);
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
      throw;
   }
   
   //----------------------------------------------------------------------
   // Check all object names in the Create command for global objects.
   // If object type is automatic global and the same object names found in
   // the GlobalObjectStore, throw an exception.
   // Just give warning for now (LOJ: 2010.12.16)
   //----------------------------------------------------------------------
   bool globalObjFound = false;
   std::string globalObjNames;
   StringArray defaultCSNames = theModerator->GetDefaultCoordinateSystemNames();
   
   for (UnsignedInt i = 0; i < objNames.size(); i++)
   {
      std::string name1 = objNames[i];
      if (find(defaultCSNames.begin(), defaultCSNames.end(), name1) != defaultCSNames.end())
      {
         std::string msg = 
            "The default CoordinateSystem \"" + name1 + "\" is an automatic "
            "global object and was already created, so ignoring";
         InterpreterException ex(msg);
         HandleError(ex, true, true);
      }
      else
      {
         GmatBase *obj1 = FindObject(name1, objTypeStrToUse);
         if (obj1 != NULL && obj1->GetIsGlobal())
         {
            globalObjFound = true;
            globalObjNames = globalObjNames + name1 + " ";
         }
      }
   }
   
   if (globalObjFound)
   {
      std::string msg = 
         "The following automatic global objects are already created, so ignoring: " + globalObjNames;
      InterpreterException ex(msg);
      HandleError(ex, true, true);
      //return false;
   }
   
   
   //-------------------------------------------------------------------
   // Create an unmanaged object and set to command
   // Note: Generally unnamed object will not be added to configuration,
   //       but we need name for Array for syntax checking, so pass name
   //       and set false to unmanage Array objects
   //-------------------------------------------------------------------
   std::string name;
   // We also need named object for celetial body, so it can be added to the
   // solar system in use with name (LOJ: 2010.04.30)
   if ((objTypeStrToUse == "Variable" || objTypeStrToUse == "Array") ||
       (find(celestialBodyList.begin(), celestialBodyList.end(), objTypeStrToUse) != 
        celestialBodyList.end()))
      name = objNames[0];
   
   #ifdef DEBUG_ASSEMBLE_CREATE
   MessageInterface::ShowMessage
      ("   About to create reference object of '%s' for Create command\n",
       objTypeStrToUse.c_str());
   #endif
   
   // We don't want to manage object to configuration, so pass 0
   GmatBase *obj = CreateObject(objTypeStrToUse, name, 0);
   
   #ifdef DEBUG_ASSEMBLE_CREATE
   MessageInterface::ShowMessage("   %s created\n", obj->GetTypeName().c_str());
   #endif
   
   if (obj == NULL)
   {
      #ifdef DEBUG_ASSEMBLE_CREATE
         MessageInterface::ShowMessage("Reference object for Create command is NULL??\n");
      #endif
      return false;
   }
   
   // Send the object to the Create command
   //cmd->SetRefObject(obj, Gmat::UNKNOWN_OBJECT, obj->GetName());
   cmd->SetRefObject(obj, GmatBase::GetObjectType(objTypeStrToUse), obj->GetName());
   
   // Special case for MatlabFunction
   // Since CallFunction does not know whether the function is Gmat or Matlab function,
   // add an extention to indicate it is MatlabFunction so that Sandbox can create
   // proper functions. Add the name to tempObjectNames so that when creating
   // CallFunction or Assignment command, it can look in the array to figure out
   // whether it is MatlabFunction or not.
   if (objTypeStrToUse == "MatlabFunction")
   {
      for (UnsignedInt i=0; i<objNames.size(); i++)
      {
         #ifdef DEBUG_ASSEMBLE_CREATE
         MessageInterface::ShowMessage
            ("   Adding '%s' to tempObjectNames\n", objNames[i].c_str());
         #endif
         tempObjectNames.push_back(objNames[i]);
      }
      
      #ifdef DEBUG_ASSEMBLE_CREATE
      MessageInterface::ShowMessage
         ("   tempObjectNames.size()=%d\n", tempObjectNames.size());
      #endif
   }
   
   #ifdef DEBUG_ASSEMBLE_CREATE
   MessageInterface::ShowMessage
      ("AssembleCreateCommand() returning true, created obj=<%p>, objType=<%s>, "
       "objName=<%s>\n", obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
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

   debugMsg = "In SetCommandRefObjects()";
   
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
   WriteStringArray("object name parts", "", parts);
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
   MessageInterface::ShowMessage
      ("   inCommandMode=%d, inRealCommandMode=%d\n", inCommandMode, inRealCommandMode);
   #endif
   
   debugMsg = "In CreateAssignmentCommand()";
   
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
// Parameter* CreateSystemParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a system Parameter from the input parameter name. If the name contains
 * dots, it consider it as a system parameter.  If it is not a system Parameter
 * it checks if object by given name is a Parameter.
 *
 * @param  name   parameter name to be parsed for Parameter creation
 *                Such as, sat1.Earth.ECC, sat1.SMA
 *
 * @return Created Paramteter pointer or pointer of the Parameter by given name
 *         NULL if it is not a system Parameter nor named object is not a Parameter
 *
 */
//------------------------------------------------------------------------------
Parameter* Interpreter::CreateSystemParameter(const std::string &str)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Interpreter::CreateSystemParameter() entered, str='%s', inFunctionMode=%d\n",
       str.c_str(), inFunctionMode);
   #endif
   
   Integer manage = 1;
   // if in function mode set manage = 2 (loj: 2008.12.16)
   if (inFunctionMode)
      manage = 2;
   
   bool paramCreated = false;
   Parameter *param = theValidator->CreateSystemParameter(paramCreated, str, manage);
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("   Parameter '%s'%screated\n", str.c_str(), paramCreated ? " " : " NOT ");
   MessageInterface::ShowMessage
      ("Interpreter::CreateSystemParameter() returning <%p><%s>'%s'\n", param,
       (param == NULL) ? "NULL" : param->GetTypeName().c_str(),
       (param == NULL) ? "NULL" : param->GetName().c_str());
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name,
//                            const std::string &ownerName, const std::string &depName)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Parameter.
 * 
 * @param  type       Type of parameter requested
 * @param  name       Name for the parameter.
 * @param  ownerName  object name of parameter requested ("")
 * @param  depName    Dependent object name of parameter requested ("")
 * 
 * @return Pointer to the constructed Parameter.
 */
//------------------------------------------------------------------------------
Parameter* Interpreter::CreateParameter(const std::string &type, 
                                        const std::string &name,
                                        const std::string &ownerName,
                                        const std::string &depName)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Interpreter::CreateParameter() type='%s', name='%s', ownerName='%s', "
       "depName='%s', inFunctionMode=%d\n", type.c_str(), name.c_str(),
       ownerName.c_str(), depName.c_str(), inFunctionMode);
   #endif
   
   Integer manage = 1;
   if (inFunctionMode)
      manage = 0;
   
   Parameter *param = theValidator->CreateParameter(type, name, ownerName, depName, manage);
   
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("Interpreter::CreateParameter() returning <%p>\n", param);
   #endif
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
   debugMsg = "In GetArrayIndex()";
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
         InterpreterException ex("\"" + name + "\" is not an Array");
         HandleError(ex);
         return NULL;
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
   #ifdef DEBUG_MAKE_ASSIGNMENT
   MessageInterface::ShowMessage
      ("\nInterpreter::MakeAssignment() lhs=<%s>, rhs=<%s>\n", lhs.c_str(), rhs.c_str());
   MessageInterface::ShowMessage
      ("   inCommandMode=%d, inRealCommandMode=%d\n", inCommandMode, inRealCommandMode);
   MessageInterface::ShowMessage
      ("   inFunctionMode=%d, hasFunctionDefinition=%d\n", inFunctionMode,
       hasFunctionDefinition);
   #endif
   
   debugMsg = "In MakeAssignment()";
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
   bool isLhsVariable = false;
   bool isRhsVariable = false;
   bool isLhsString = false;
   bool isRhsString = false;
   bool isRhsNumber = false;
   
   currentBlock = lhs + " = " + rhs;
   
   #ifdef DEBUG_MAKE_ASSIGNMENT
   WriteStringArray("lhs parts", "", lhsParts);
   WriteStringArray("rhs parts", "", rhsParts);
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
            InterpreterException ex("Object field assignment is incomplete");
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
         
         if (lhsObj->IsOfType("Variable"))
            isLhsVariable = true;
         else if (lhsObj->IsOfType("String"))
            isLhsString = true;
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
   
   #ifdef DEBUG_MAKE_ASSIGNMENT
   MessageInterface::ShowMessage
      ("   isLhsObject=%d, isLhsArray=%d, isLhsVariable=%d, isLhsString=%d, "
       "lhsPropName=<%s>, lhsObj=<%p><%s>\n", isLhsObject, isLhsArray,
       isLhsVariable, isLhsString, lhsPropName.c_str(), lhsObj,
       (lhsObj == NULL) ? "NULL" : lhsObj->GetName().c_str() );
   #endif
   
   // check RHS
   if (rhsPartCount > 1)
   {
      rhsObjName = rhsParts[0];
      std::string objTypeStr = "";
      // Check if RHS has open paren, then it should be an Array (loj: 2008.08.15)
      if (rhsObjName.find_first_of("(") != rhsObjName.npos)
         objTypeStr = "Array";
      rhsObj = FindObject(rhsObjName, objTypeStr);
      
      if (rhsObj == NULL)
      {
         //throw InterpreterException("Cannot find RHS object: " + rhsObjName + "\n");
         
         #ifdef DEBUG_MAKE_ASSIGNMENT
         MessageInterface::ShowMessage
            ("   Cannot find RHS object '%s' of type <%s>. It may be a string value\n",
             rhsObjName.c_str(), objTypeStr.c_str());
         #endif
      }
      else
      {
         // Note: Do not set rhsObj to true here since it needs to create
         // a Parameter if needed.
         
         #ifdef DEBUG_MAKE_ASSIGNMENT
         MessageInterface::ShowMessage
            ("   Found rhs object <%s>'%s', now checking for dot\n",
             rhsObj->GetTypeName().c_str(), rhsObj->GetName().c_str());
         #endif
         
         // Check if it is CallFunction first
         dot = rhs.find('.');
         if (dot == rhs.npos)
         {
            rhsPropName = rhsParts[1];
         }
         else
         {
            std::string afterDot = rhs.substr(dot+1);
            // Check if rhs is a Parameter first
            // This will fix Bug 1669 (LOJ: 2009.12.08)
            if (theValidator->IsParameterType(rhs))
            {
               rhsPropName = afterDot;
            }
            else
            {
               // Check if it is object property
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
         if (rhsObj->IsOfType("Variable"))
            isRhsVariable = true;
         else if (rhsObj->IsOfType("String"))
            isRhsString = true;
         
         if (IsArrayElement(rhs))
            isRhsArray = true;
         else
         {
            // @note
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
      else
      {
         if (GmatStringUtil::IsNumber(rhs))
            isRhsNumber = true;
      }
   }
   
   #ifdef DEBUG_MAKE_ASSIGNMENT
   MessageInterface::ShowMessage
      ("   isRhsObject=%d, isRhsArray=%d, isRhsVariable=%d, isRhsString=%d, "
       "isRhsNumber=%d, rhsPropName=<%s>, rhsObj=<%p><%s>\n",
       isRhsObject, isRhsArray, isRhsVariable, isRhsString, isRhsNumber,
       rhsPropName.c_str(), rhsObj,
       (rhsObj == NULL) ? "NULL" : rhsObj->GetName().c_str() );
   #endif
   
   if (isLhsObject)
   {
      bool isAllowed = true;
      
      // Variable is allowed to set to only numbers (Bug 2043)
      if (isLhsVariable && !isRhsNumber)
         isAllowed = false;
      
      // String is allowed to set to only literals (Bug 2043)
      if (isAllowed && isLhsString && isRhsString)
         isAllowed = false;
      
      if (!isAllowed)
      {
         InterpreterException ex
            ("Setting \"" + lhs + "\" to \"" + rhs + "\" is not allowed before BeginMissionSequence");
         HandleError(ex);
         return false;
      }
      
      if (isRhsObject)
         retval = SetObjectToObject(lhsObj, rhsObj, rhs);
      else if (rhsPropName != "")
         retval = SetObjectToProperty(lhsObj, rhsObj, rhsPropName);
      else if (isRhsArray)
         retval = SetObjectToArray(lhsObj, rhs);
      else
         retval = SetObjectToValue(lhsObj, rhs);
   }
   else if (lhsPropName != "") // LHS is object property
   {
      bool isAllowed = true;
      GmatBase *toObj = NULL;
      Integer toId = -1;
      Gmat::ParameterType toType;
      
      // Check LHS property type
      FindPropertyID(lhsObj, lhsPropName, &toObj, toId, toType);
      
      // Check invalid object id
      // We don't allow setting Parameters in resource mode, so write an error message
      // (LOJ: 2013.03.18 GMT-3155 FIX)
      if (toId == -1)
      {
         // Special case for spacecraft epoch, since we can set "sat.Epoch.UTCGregorian" 
         // instead of "sat.Epoch = 01 Jan 2000 11:59:27.966" and "sat.DateFormat = UTCGregorian"
         // Special case for drag force model, fm.Drag.AtmosphereModel.
         if (lhs.find(".Epoch.") == lhs.npos && lhs.find(".Drag.") == lhs.npos)
         {
            InterpreterException ex
               ("The field name \"" + lhsPropName + "\" on object \"" + lhsObjName + "\" is not permitted");
            HandleError(ex);
            return false;
         }
      }
      
      // Only object type of property is allowed to set to another object
      if (toType != Gmat::OBJECT_TYPE && toType != Gmat::OBJECTARRAY_TYPE)
      {
         // Setting object property to Variable, Array and String are not allowd(Bug 2043)
         if (isRhsArray || isRhsVariable || isRhsString)
            isAllowed = false;
         
         // Setting object property to other property is not allowed(Bug 2043)
         // excluding FILENAME_TYPE which can have dots.
         if (isAllowed && rhsPropName != "" && toType != Gmat::FILENAME_TYPE)
         {
            isAllowed = false;
         }
      }
      
      if (!isAllowed)
      {
         InterpreterException ex
            ("Setting \"" + lhs + "\" to \"" + rhs + "\" is not allowed before BeginMissionSequence");
         HandleError(ex);
         return false;
      }
      
      if (isRhsObject)
         retval = SetPropertyToObject(lhsObj, lhsPropName, rhsObj);
      else if (rhsPropName != "")
         retval = SetPropertyToProperty(lhsObj, lhsPropName, rhsObj, rhsPropName);
      else if (isRhsArray)
         retval = SetPropertyToArray(lhsObj, lhsPropName, rhs);
      else
         retval = SetPropertyToValue(lhsObj, lhsPropName, rhs);
   }
   else if (isLhsArray)
   {
      if (!isRhsNumber)
      {
         InterpreterException ex
            ("Setting \"" + lhs + "\" to \"" + rhs + "\" is not allowed before BeginMissionSequence");
         HandleError(ex);
         return false;
      }
      
      if (isRhsObject)
         retval = SetArrayToObject(lhsObj, lhs, rhsObj);
      else if (rhsPropName != "")
         retval = SetArrayToProperty(lhsObj, lhs, rhsObj, rhsPropName);
      else if (isRhsArray)
         retval = SetArrayToArray(lhsObj, lhs, rhsObj, rhs);
      else
         retval = SetArrayToValue(lhsObj, lhs, rhs);
   }
   else
   {
      InterpreterException ex
         ("Interpreter::MakeAssignment() Internal error if it reached here.");
      HandleError(ex);
   }
   
   #ifdef DEBUG_MAKE_ASSIGNMENT
   MessageInterface::ShowMessage
      ("Interpreter::MakeAssignment() returning lhsObj=%p\n", lhsObj);
   #endif
   
   if (retval)
      return lhsObj;
   else
      return NULL;
}


//-------------------------------------------------------------------------------
// bool SetObjectToObject(GmatBase *toObj, GmatBase *fromObj, const std::string &rhs)
//-------------------------------------------------------------------------------
bool Interpreter::SetObjectToObject(GmatBase *toObj, GmatBase *fromObj,
                                    const std::string &rhs)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToObject() to=%s, from=%s, rhs='%s'\n",
       toObj->GetName().c_str(), fromObj->GetName().c_str(), rhs.c_str());
   #endif
   
   debugMsg = "In SetObjectToObject()";
   
   // If in object initialization mode, do not allow object to object assignment
   // except spacecraft (GMT-3534)
   if (!inCommandMode)
   {
      if (!(toObj->IsOfType(Gmat::SPACECRAFT)) && !(fromObj->IsOfType(Gmat::SPACECRAFT)))
      {
         InterpreterException ex
            ("Setting \"" + toObj->GetName() + "\" to \"" + rhs +
             "\" is not allowed before BeginMissionSequence");
         HandleError(ex);
         return false;
      }
   }
   
   // Copy object
   if (toObj->GetTypeName() == fromObj->GetTypeName())
   {
      toObj->Copy(fromObj);
   }
   else
   {
      //InterpreterException ex("Object type of LHS and RHS are not the same.");
      InterpreterException ex
         ("Setting \"" + toObj->GetName() + "\" to \"" + rhs + "\" is not allowed before BeginMissionSequence");
      HandleError(ex);
      return false;
   }
   
   // More handling for Variable
   if (toObj->IsOfType("Variable"))
   {
      // If first char is - sign, negate the value
      if (rhs[0] == '-')
      {
         Real rval = toObj->GetRealParameter("Value") * -1;
         toObj->SetRealParameter("Value", rval);
      }
      // Set Variable's InitialValue so when it is written out, it will have original string value (LOJ: 2010.09.21)
      toObj->SetStringParameter("InitialValue", rhs);
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToObject() returning true\n");
   #endif
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetObjectToProperty(GmatBase *toObj, GmatBase *fromOwner,
//                          const std::string &fromProp)
//-------------------------------------------------------------------------------
bool Interpreter::SetObjectToProperty(GmatBase *toObj, GmatBase *fromOwner,
                                      const std::string &fromProp)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetObjectToProperty() toObj=%s, fromOwner=%s, fromProp=%s\n",
       toObj->GetName().c_str(), fromOwner->GetName().c_str(), fromProp.c_str());
   #endif
   
   debugMsg = "In SetObjectToProperty()";
   std::string rhs = fromOwner->GetName() + "." + fromProp;
   Integer fromId = -1;
   Gmat::ParameterType fromType = Gmat::UNKNOWN_PARAMETER_TYPE;
   Parameter *rhsParam = NULL;
   
   if (toObj->GetTypeName() != "Variable" && toObj->GetTypeName() != "String")
   {
      InterpreterException ex
         ("Setting an object \"" + toObj->GetName() + "\" to " + fromProp +
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
      
      // it is not a Parameter, so handle error
      if (rhsParam == NULL)
      {
         // Try setting as Variable expression (loj: 2008.08.05)
         // to handle var = sat.A1ModJulian - GmatTimeConstants::MJD_OF_J2000 prior to mission sequence
         // It also shows correct expression in the GUI
         if (ParseVariableExpression((Parameter*)toObj, rhs))
            return true;
         else
         {
            HandleError(e);
            return false;
         }
      }
      
      fromType = rhsParam->GetReturnType();
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("SetObjectToProperty() rhs:%s is a parameter\n", rhs.c_str());
      #endif
   }
   
   Parameter *toParam = (Parameter*)toObj;
   Gmat::ParameterType toType = toParam->GetReturnType();
   
   if (fromType == toType)
   {
      if (fromId == -1)
      {
         // LHS is a Variable or String, RHS is a Parameter
         if (toType == Gmat::STRING_TYPE || toType == Gmat::ENUMERATION_TYPE ||
             toType == Gmat::FILENAME_TYPE)
            toObj->SetStringParameter("Value", rhsParam->GetString());
         else if (toType == Gmat::REAL_TYPE)
            ParseVariableExpression(toParam, rhs);
      }
      else
      {
         // LHS is a Variable or String, RHS is an ObjectProperty
         if (toType == Gmat::STRING_TYPE || toType == Gmat::ENUMERATION_TYPE ||
             toType == Gmat::FILENAME_TYPE)
            toObj->SetStringParameter("Value", fromOwner->GetStringParameter(fromId));
         else if (toType == Gmat::REAL_TYPE)
         {
            // Check to see if fromProp is also a system Parameter, (loj: 2008.08.06)
            // if so Parameter takes higher presendence over ObjectProperty.
            rhsParam = CreateSystemParameter(rhs);
            if (rhsParam != NULL)
            {
               ParseVariableExpression(toParam, rhs);
            }
            else
               toObj->SetRealParameter("Value", fromOwner->GetRealParameter(fromId));
         }
      }
   }
   else
   {
      //InterpreterException ex
      //   ("Setting an object \"" + toObj->GetName() + "\" to " + fromProp +
      //    "\" is not allowed");
      InterpreterException ex
         ("Setting \"" + fromProp + "\" to an object \"" + toObj->GetName() +
          "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetObjectToArray(GmatBase *toObj, const std::string &fromArray)
//-------------------------------------------------------------------------------
bool Interpreter::SetObjectToArray(GmatBase *toObj, const std::string &fromArray)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToArray() toObj=%s, fromArray=%s\n",
       toObj->GetName().c_str(), fromArray.c_str());
   #endif
   
   debugMsg = "In SetObjectToArray()";
   
   if (toObj->GetTypeName() != "Variable")
   {
      //InterpreterException ex
      //   ("Setting \"" + fromArray + "\" to an object \"" + toObj->GetName() +
      //    "\" is not allowed");
      InterpreterException ex
         ("Setting \"" + toObj->GetName() + "\" to an array \"" + fromArray +
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
      ("   SetObjectToArray() row=%d, col=%d\n", row, col);
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
   MessageInterface::ShowMessage("   SetObjectToArray() rval=%f\n", rval);
   #endif

   try
   {
      // Handle minus sign
      if (fromArray[0] == '-')
         rval = -rval;
      toObj->SetRealParameter("Value", rval);
   }
   catch (BaseException &e)
   {
      HandleError(e);
      return false;
   }
   
   // Set Variable's InitialValue so when it is written out, it will have original string value (LOJ: 2010.09.21)
   toObj->SetStringParameter("InitialValue", fromArray);
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetObjectToValue(GmatBase *toObj, const std::string &value)
//-------------------------------------------------------------------------------
bool Interpreter::SetObjectToValue(GmatBase *toObj, const std::string &value)
{
   debugMsg = "In SetObjectToValue()";
   std::string toObjType = toObj->GetTypeName();
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToValue() toObjType=<%s>, toObjName=%s, value=<%s>\n",
       toObjType.c_str(), toObj->GetName().c_str(), value.c_str());
   #endif
   
   if (toObjType != "Variable" && toObjType != "String" && toObjType != "Array")
   {
      InterpreterException ex
         ("Setting an object \"" + toObj->GetName() + "\" of type \"" + toObjType +
          "\" to a value \"" + value + "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   if (toObjType == "Array")
   {
      // Check if array is one element array
      std::string desc = toObj->GetStringParameter("Description");
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage("   Array = '%s'\n", desc.c_str());
      #endif
      if (GmatStringUtil::IsOneElementArray(desc))
      {
         // Replace [] to () so that value can be assigned
         std::string initialValueStr = GmatStringUtil::Replace(desc, "[", "(");
         initialValueStr = GmatStringUtil::Replace(initialValueStr, "]", ")");
         initialValueStr = initialValueStr + "=" + value;
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("   Setting initial value to '%s'\n", initialValueStr.c_str());
         #endif
         toObj->SetStringParameter("InitialValue", initialValueStr);
      }
      else
      {
         InterpreterException ex
            ("Setting an object \"" + toObj->GetName() + "\" of type \"" + toObjType +
             "\" to a value \"" + value + "\" is not allowed");
         HandleError(ex);
         return false;
      }
   }
   else if (toObjType == "String")
   {
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage("   Checking if value is enclosed with quotes\n");
      #endif
      
      // check for unpaired single quotes
      if (GmatStringUtil::HasMissingQuote(value, "'"))
      {
         InterpreterException ex("The string \"" + value + "\" has missing single quote");
         HandleError(ex);
         return false;
      }
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage("   It is enclosed with quotes, now removing trailing spaces\n");
      #endif
      
      // Remove trailing spaces
      std::string valueToUse = GmatStringUtil::Trim(value, GmatStringUtil::TRAILING);
      valueToUse = GmatStringUtil::RemoveEnclosingString(valueToUse, "'");
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("   Calling %s->SetStringParameter(Expression, %s)\n", toObj->GetName().c_str(),
          valueToUse.c_str());
      MessageInterface::ShowMessage
         ("   Calling %s->SetStringParameter(Value, %s)\n", toObj->GetName().c_str(),
          valueToUse.c_str());
      #endif
      
      toObj->SetStringParameter("Expression", valueToUse);
      toObj->SetStringParameter("Value", valueToUse);
   }
   else if (toObjType == "Variable")
   {
      Real rval;

      try
      {
         if (GmatStringUtil::ToReal(value, rval, true))
         {      
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage("   SetObjectToValue() rval=%f\n", rval);
            #endif
            
            toObj->SetRealParameter("Value", rval);
         }
         else
         {
            // For bug 2025 fix, commented out (LOJ: 2010.09.16)
            //if (!ParseVariableExpression((Parameter*)toObj, value))
            //{
            //InterpreterException ex
            //("Setting \"" + value + "\" to a Variable \"" + toObj->GetName() +
            //"\" is not allowed");
            InterpreterException ex
               ("Setting an object \"" + toObj->GetName() + "\" of type \"" + toObjType +
                "\" to a value \"" + value + "\" is not allowed");
            HandleError(ex);
            return false;
            //}
         }
      }
      catch (BaseException &e)
      {
         HandleError(e);
         return false;
      }
   }
   
   // Set Variable's InitialValue so when it is written out, it will have original string value (LOJ: 2010.09.21)
   toObj->SetStringParameter("InitialValue", value);
   return true;
}


//-------------------------------------------------------------------------------
// bool SetPropertyToObject(GmatBase *toOwner, const std::string &toProp,
//                          GmatBase *fromObj)
//-------------------------------------------------------------------------------
bool Interpreter::SetPropertyToObject(GmatBase *toOwner, const std::string &toProp,
                                      GmatBase *fromObj)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyToObject() ownerType=%s, toOwner=%s, toProp=%s, "
       "fromObj=%s\n", toOwner->GetTypeName().c_str(), toOwner->GetName().c_str(),
       toProp.c_str(), fromObj->GetName().c_str());
   #endif
   
   debugMsg = "In SetPropertyToObject()";
   
   if (toOwner->GetType() == Gmat::ODE_MODEL)
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
            ("Interpreter::SetPropertyToObject() returning true, added to delayed "
             "blocks: line:%s, %s\n", lineNumStr.c_str(), currentBlock.c_str());
         #endif
         
         return true;
      }
   }
   catch (BaseException &)
   {
      if (parsingDelayedBlock)
      {
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("Interpreter::SetPropertyToObject() returning false,  Caught exception "
             "in parsing delayed block mode\n");
         #endif
         return false;
      }
      
      delayedBlocks.push_back(currentBlock);
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("Interpreter::SetPropertyToObject() returning true, added to delayed "
          "blocks: %s\n", currentBlock.c_str());
      #endif
      
      return true;
   }
   
   toType = toObj->GetParameterType(toId);
   
   // Let's treat enumeration and filename type as string type
   if (toType == Gmat::ENUMERATION_TYPE || toType == Gmat::FILENAME_TYPE)
      toType = Gmat::STRING_TYPE;
   
   try
   {
      std::string fromTypeName = fromObj->GetTypeName();
      bool continueNext = false;
      
      // Handle if RHS is Parameter
      if (fromObj->GetType() == Gmat::PARAMETER)
      {        
         Parameter *fromParam = (Parameter*)fromObj;
         Gmat::ParameterType fromType = fromParam->GetReturnType();
         Gmat::ObjectType toObjPropObjType = toObj->GetPropertyObjectType(toId);
         
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("   From object is a Parameter, toId=%d, fromType=%d, toType=%d\n",
             toId, fromType, toType);
         #endif
         
         if (toType == Gmat::OBJECT_TYPE && toObjPropObjType == Gmat::PARAMETER)
            toObj->SetStringParameter(toId, fromObj->GetName());
         else
         {
            InterpreterException ex
               ("The value of \"" + fromObj->GetName() + "\" for field \"" + toProp +
                "\" on object " + "\"" + toOwner->GetName() + "\" is not an allowed value");
            HandleError(ex);
            return false;
         }
         
         //================================================================
         // Currently setting object field to Parameter is not allowed.
         // @see FRAC-10 ScriptLanguage
         // Only literal assignments are allowed in initialization section;
         // no execution of commands or evaluation of parameters is done
         //================================================================
         /*
         if (fromType == toType)
         {
            if (toType == Gmat::STRING_TYPE)
               toObj->SetStringParameter(toId, fromObj->GetStringParameter("Value"));
            else if (toType == Gmat::REAL_TYPE)
               toObj->SetRealParameter(toId, fromObj->GetRealParameter("Value"));
            else
               continueNext = true;
         }
         else
         {
            bool success = true;
            if (fromTypeName == "String")
            {
               std::string fromValueToUse = fromParam->GetString();
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage("   fromValueToUse='%s'\n", fromValueToUse.c_str());
               #endif
               if (toType == Gmat::STRING_TYPE || toType == Gmat::STRINGARRAY_TYPE)
                  success = toObj->SetStringParameter(toId, fromObj->GetStringParameter("Value"));
               else if (toType == Gmat::OBJECT_TYPE || toType == Gmat::OBJECTARRAY_TYPE)
                  success = toObj->SetStringParameter(toId, fromObj->GetName());
               else
                  success = false;
            }
            else if (fromTypeName == "Variable")
            {
               if (toType == Gmat::REAL_TYPE)
                  toObj->SetRealParameter(toId, fromObj->GetRealParameter("Value"));
               // Added to fix GMAT XYPlot1.IndVar = Var; (loj: 2008.08.01)
               //else if (toType == Gmat::OBJECT_TYPE && toObj->IsOfType(Gmat::XY_PLOT))
               else if (toType == Gmat::OBJECT_TYPE && toObjPropObjType == Gmat::PARAMETER)
                  toObj->SetStringParameter(toId, fromObj->GetName());
               else
                  success = false;
            }
            else
            {
               if (toType == Gmat::OBJECT_TYPE || toType == Gmat::OBJECTARRAY_TYPE)
                  toObj->SetStringParameter(toId, fromObj->GetName());
               else
                  success = false;
            }
            
            if (!success)
            {
               InterpreterException ex
                  ("The value of \"" + fromObj->GetName() + "\" for field \"" + toProp +
                   "\" on object " + "\"" + toOwner->GetName() + "\" is not an allowed value");
               HandleError(ex);
               return false;
            }
            else
               continueNext = true;
         }
         */
         //================================================================
      }
      else
      {
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("   Setting objType=%s, objName=%s\n", fromTypeName.c_str(),
             fromObj->GetName().c_str());
         #endif
         
         bool success = false;
         Gmat::ObjectType objPropType = Gmat::UNKNOWN_OBJECT;
         
         // Check if object name is valid object type
         if (toObj->IsOwnedObject(toId))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage("   '%s' is owned object\n", toProp.c_str());
            #endif
            toObj->SetStringParameter(toProp, fromObj->GetName());
            toObj->SetRefObject(fromObj, fromObj->GetType(), fromObj->GetName());
            objPropType = toObj->GetPropertyObjectType(toId);
            if (objPropType == Gmat::UNKNOWN_OBJECT || 
                (objPropType != Gmat::UNKNOWN_OBJECT && fromObj->IsOfType(objPropType)))
            {
               success = true;
               // Since CoordinateSystem::SetRefObject() clones AxisSystem, delete it from here
               if (toObj->GetType() == Gmat::COORDINATE_SYSTEM &&
                   (fromObj->GetType() == Gmat::AXIS_SYSTEM))
               {
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (fromObj, "oldLocalAxes", "Interpreter::SetPropertyToObject()",
                      "deleting oldLocalAxes");
                  #endif
                  delete fromObj;
                  fromObj = NULL;
               }
            }
         }
         else
         {
            toObj->SetStringParameter(toProp, fromObj->GetName());
            objPropType = toObj->GetPropertyObjectType(toId);
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   '%s' is not owned object, objPropType=%d<%s>, fromObjTypeName='%s'\n",
                toProp.c_str(), objPropType, GmatBase::GetObjectTypeString(objPropType).c_str(),
                fromObj->GetTypeName().c_str());
            #endif
            if (objPropType == Gmat::UNKNOWN_OBJECT || 
                (objPropType != Gmat::UNKNOWN_OBJECT && fromObj->IsOfType(objPropType)))
            {
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling SetStringParameter(%s, %s)\n", toProp.c_str(),
                   fromObj->GetName().c_str());
               #endif
               success = true;
            }
         }

         // Setting was not successful, handle error message
         if (!success)
         {
            // Ignore error if GetPropertyObjectType() returns Gmat::UNKNOWN_OBJECT.
            // Since not all classes implemented GetPropertyObjectType(), GmatBase
            // returns Gmat::UNKNOWN_OBJECT
            if (objPropType != Gmat::UNKNOWN_OBJECT)
            {
               InterpreterException ex
                  ("The value of \"" + fromObj->GetName() + "\" for field \"" + toProp +
                   "\" on object " + "\"" + toOwner->GetName() + "\" is not an allowed value");
               HandleError(ex);
               return false;
            }
         }
      }
   }
   catch (BaseException &ex)
   {
      HandleError(ex);
      return false;
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyToObject() returning true\n");
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
   debugMsg = "In SetPropertyToProperty()";
   bool retval = true;
   errorMsg1 = "";
   errorMsg2 = "";
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetPropertyToProperty() toOwner=%s<%s>, toProp=<%s>, fromOwner=<%s>, fromProp=<%s>\n",
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
   catch (BaseException &)
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
   catch (BaseException &)
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
      SetObjectToObject(lhsParam, rhsParam, fromProp);
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
         if (toType == Gmat::STRING_TYPE || toType == Gmat::ENUMERATION_TYPE ||
             toType == Gmat::FILENAME_TYPE)
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
            (errorMsg1 + "for field \"" + toProp + "\" on object " + "\"" +
             toOwner->GetName() + "\" is not an allowed value" + errorMsg2);
         HandleError(ex);
      }
   }
   
   return retval;
}


//-------------------------------------------------------------------------------
// bool SetPropertyToArray(GmatBase *toOwner, const std::string &toProp,
//                         const std::string &fromArray)
//-------------------------------------------------------------------------------
bool Interpreter::SetPropertyToArray(GmatBase *toOwner, const std::string &toProp,
                                     const std::string &fromArray)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyToArray() toOwner=%s, toProp=%s, fromArray=%s\n",
       toOwner->GetName().c_str(), toProp.c_str(), fromArray.c_str());
   #endif
   
   debugMsg = "In SetPropertyToArray()";
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
   
   // This is fix for allowing array elemement to object type,
   // such as MyXyPlot.XVariable = MyArray(2,2). (GMT-2370 LOJ: 2012.08.03)
   // If lhs is object type, set as string
   if (toType == Gmat::OBJECT_TYPE)
   {
      try
      {
         toOwner->SetStringParameter(toId, fromArray);
         return true;
      }
      catch (BaseException &e)
      {
         HandleError(e);
         return false;
      }
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
      ("Interpreter::SetPropertyToArray() exiting. rval=%f, row=%d, col=%d, \n",
       rval, row, col);
   #endif
   
   return true;
}


//-------------------------------------------------------------------------------
// bool SetPropertyToValue(GmatBase *toOwner, const std::string &toProp,
//                         const std::string &value)
//-------------------------------------------------------------------------------
bool Interpreter::SetPropertyToValue(GmatBase *toOwner, const std::string &toProp,
                                     const std::string &value)
{
   debugMsg = "In SetPropertyToValue()";
   bool retval = false;
   errorMsg1 = "";
   errorMsg2 = "";
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyToValue() entered, objType=<%s>, objName=<%s>, toProp=<%s>, "
       "value=<%s>\n", toOwner->GetTypeName().c_str(), toOwner->GetName().c_str(),
       toProp.c_str(), value.c_str());
   #endif
   
   if (toOwner->GetType() == Gmat::ODE_MODEL)
   {
      retval = SetForceModelProperty(toOwner, toProp, value, NULL);
   }
   else if (toOwner->GetType() == Gmat::MEASUREMENT_MODEL)
   {
      retval = SetMeasurementModelProperty(toOwner, toProp, value);
   }
   else if ((toOwner->GetType() == Gmat::DATASTREAM) ||
            (toOwner->GetType() == Gmat::DATA_FILE))
   {
      retval = SetDataStreamProperty(toOwner, toProp, value);
   }
   else if (toOwner->GetType() == Gmat::SOLAR_SYSTEM)
   {
      retval = SetSolarSystemProperty(toOwner, toProp, value);
   }
   else
   {
      StringArray parts = theTextParser.SeparateDots(toProp);
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("   There are %d part(s) after separating by dots\n", parts.size());
      for (UnsignedInt i = 0; i < parts.size(); i++)
         MessageInterface::ShowMessage("   parts[%d] = <%s>\n", i, parts[i].c_str());
      #endif
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
         
         if (toId == Gmat::PARAMETER_REMOVED)
         {
            InterpreterException ex
               ("The field name \"" + toProp + "\" on object " + "\"" +
                toOwner->GetName() + "\" is no longer in use");
            HandleError(ex, true, true);
            ignoreError = true;
            return false;
         }
                  
         if (toObj == NULL)
         {
            if (parsingDelayedBlock)
            {
               InterpreterException ex
                  ("The field name \"" + toProp + "\" on object \"" + toOwner->GetName() +
                   "\" is not permitted");
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
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   retval=%d, ignoreError=%d\n", retval, ignoreError);
   #endif
   if (retval == false && !ignoreError)
   {
      if (errorMsg1 == "")
      {
         InterpreterException ex
            ("The value of \"" + value + "\" for field \"" + toProp + "\" on object " + "\"" +
             toOwner->GetName() + "\" is not permitted");
         HandleError(ex);
      }
      else
      {
         InterpreterException ex
            (errorMsg1 + "for field \"" + toProp + "\" on object " + "\"" +
             toOwner->GetName() + "\" is not an allowed value" + errorMsg2);
         HandleError(ex);
      }
   }
   
   if (ignoreError)
      ignoreError = false;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyToValue() returning retval=%d\n", retval);
   #endif
   
   return retval;
}


//-------------------------------------------------------------------------------
// bool SetArrayToObject(GmatBase *toArrObj, const std::string &toArray,
//                       GmatBase *fromObj)
//-------------------------------------------------------------------------------
bool Interpreter::SetArrayToObject(GmatBase *toArrObj, const std::string &toArray,
                                   GmatBase *fromObj)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToObject() toArrObj=%s, toArray=%s, fromObj=%s\n",
       toArrObj->GetName().c_str(), toArray.c_str(), fromObj->GetName().c_str());
   #endif
   
   debugMsg = "In SetArrayToObject()";
   
   if (fromObj->GetTypeName() != "Variable")
   {
      //InterpreterException ex
      //   ("Cannot set object other than Variable or Array element.");
      //InterpreterException ex
      //   ("Setting object \"" + fromObj->GetName() + "\" to an array \"" + toArray +
      //    "\" is not permitted.");
      InterpreterException ex
         ("Setting an array \"" + toArray + "\" to an object \"" + fromObj->GetName() + 
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
      ("   SetArrayToObject()rval=%f, row=%d, col=%d\n", rval, row, col);
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
   
   // Set InitialValue so when it is written out, it will have original string value (LOJ: 2010.09.21)
   toArrObj->SetStringParameter("InitialValue", toArray + "=" + fromObj->GetName());
   return true;
}


//-------------------------------------------------------------------------------
// bool SetArrayToProperty(GmatBase *toArrObj, const std::string &toArray,
//                         GmatBase *fromOwner, const std::string &fromProp)
//-------------------------------------------------------------------------------
bool Interpreter::SetArrayToProperty(GmatBase *toArrObj, const std::string &toArray,
                                     GmatBase *fromOwner, const std::string &fromProp)
{
   #ifdef DEBGU_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToProperty() toArrObj=%s, toArray=%s, fromOwner=%s, "
       "fromProp=%s\n", toArrObj->GetName().c_str(), toArray.c_str(),
       fromOwner->GetName().c_str(), fromProp.c_str());
   #endif
   
   debugMsg = "In SetArrayToProperty()";
   
   // get object parameter id
   Integer fromId = fromOwner->GetParameterID(fromProp);
   
   if (fromOwner->GetParameterType(fromId) != Gmat::REAL_TYPE)
   {
      //InterpreterException ex
      //   ("Setting non-Real type of \"" + fromProp + "\" to an Array element \"" +
      //    toArray + "\" is not allowed");
      InterpreterException ex
         ("Setting an array element \"" + toArray + "\" to \"" + fromProp +
          "\" is not allowed");
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
      ("   SetArrayToProperty()rval=%f, row=%d, col=%d\n", rval, row, col);
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
   
   // Set InitialValue so when it is written out, it will have original string value (LOJ: 2010.09.21)
   toArrObj->SetStringParameter("InitialValue", toArray + "=" + fromProp);
   return true;
}


//-------------------------------------------------------------------------------
// bool SetArrayToArray(GmatBase *toArrObj, const std::string &toArray,
//                      GmatBase *fromArrObj, const std::string &fromArray)
//-------------------------------------------------------------------------------
/**
 * Sets Array to Array, such as toArray = fromArray
 */
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
   
   debugMsg = "In SetArrayToArray()";
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
      ("   SetArrayToArray() rowTo=%d, colTo=%d\n", rowTo, colTo);
   #endif
   
   try
   {
      if (fromArray[0] == '-')
         toArrObj->SetRealParameter("SingleValue", -rval, rowTo, colTo);
      else   
         toArrObj->SetRealParameter("SingleValue", rval, rowTo, colTo);
   }
   catch (BaseException &e)
   {
      HandleError(e);
      return false;
   }
   
   // Set InitialValue so when it is written out, it will have original string value (LOJ: 2010.09.21)
   toArrObj->SetStringParameter("InitialValue", toArray + "=" + fromArray);
   return true;
}


//-------------------------------------------------------------------------------
// bool SetArrayToValue(GmatBase *array, const std::string &toArray,
//                      const std::string &value)
//-------------------------------------------------------------------------------
bool Interpreter::SetArrayToValue(GmatBase *array, const std::string &toArray,
                                  const std::string &value)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToValue() array=%s, toArray=%s, value=%s\n",
       array->GetName().c_str(), toArray.c_str(), value.c_str());
   #endif
   
   debugMsg = "In SetArrayToValue()";
   Integer row, col;
   Real rval;
   
   Parameter *param = GetArrayIndex(toArray, row, col);
   if (param == NULL)
      return false;
   
   if (GmatStringUtil::ToReal(value, rval, true))
   {
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("   SetArrayToValue() rval=%f, row=%d, col=%d\n", rval, row, col);
      #endif

      try
      {
         array->SetRealParameter("SingleValue", rval, row, col);
      }
      catch (BaseException &)
      {
         InterpreterException ex("Index exceeds matrix dimensions");
         HandleError(ex);
         return false;
      }
   }
   else
   {
      //InterpreterException ex
      //   ("Setting \"" + value + "\" to an object \"" + toArray +
      //    "\" is not allowed");
      InterpreterException ex
         ("Setting an object \"" + toArray + "\" to \"" + value +
          "\" is not allowed");
      HandleError(ex);
      return false;
   }
   
   // Set InitialValue so when it is written out, it will have original string value (LOJ: 2010.09.21)
   array->SetStringParameter("InitialValue", toArray + "=" + value);
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
                                   const std::string &value,
                                   const Integer index, const Integer colIndex)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyValue() entered, obj=<%s>, id=%d, type=%d, value=<%s>, index=%d\n",
       obj->GetName().c_str(), id, type, value.c_str(), index);
   #endif
   
   debugMsg = "In SetPropertyValue()";
   bool retval = false;
   std::string valueToUse = value;
   CheckForSpecialCase(obj, id, valueToUse);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   propertyType=%s, valueToUse=<%s>\n",
       type == -1 ? "UNKNOWN_TYPE" : GmatBase::PARAM_TYPE_STRING[type].c_str(),
       valueToUse.c_str());
   #endif
   
   if (type == -1)
      return false;
   
   switch (type)
   {
   case Gmat::OBJECT_TYPE:
   case Gmat::OBJECTARRAY_TYPE:
      {
         return SetPropertyObjectValue(obj, id, type, valueToUse, index);
      }
   case Gmat::ENUMERATION_TYPE:
   case Gmat::FILENAME_TYPE:
   case Gmat::STRING_TYPE:
   case Gmat::STRINGARRAY_TYPE:
      {
         return SetPropertyStringValue(obj, id, type, valueToUse, index);
      }
   case Gmat::INTEGER_TYPE:
      {
         Integer ival;
         if (GmatStringUtil::ToInteger(valueToUse, ival))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling '%s'->SetIntegerParameter(%d, %d)\n",
                obj->GetName().c_str(), id, ival);
            #endif
            
            obj->SetIntegerParameter(id, ival);
            retval = true;
         }
         else
         {
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            errorMsg2 = " Only integer number is allowed";
         }
         break;
      }
   case Gmat::UNSIGNED_INT_TYPE:
      {
         UnsignedInt ival;
         if (GmatStringUtil::ToUnsignedInt(valueToUse, ival))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling '%s'->SetUnsignedIntParameter(%d, %d)\n",
                obj->GetName().c_str(), id, ival);
            #endif
            
            obj->SetUnsignedIntParameter(id, ival);
            retval = true;
         }
         else
         {
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            errorMsg2 = " Only unsigned integer number is allowed";
         }
         break;
      }
   case Gmat::UNSIGNED_INTARRAY_TYPE:
      {
         UnsignedInt ival;
         if (GmatStringUtil::ToUnsignedInt(valueToUse, ival))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling '%s'->SetUnsignedIntParameter(%d, %d, %d)\n",
                obj->GetName().c_str(), id, ival, index);
            #endif
            
            obj->SetUnsignedIntParameter(id, ival, index);
            retval = true;
         }
         else
         {
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            errorMsg2 = " Only unsigned integer number is allowed";
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
               GmatStringUtil::ToString(rval, false, false, false, 15, 1);
            MessageInterface::ShowMessage
               ("   Calling <%s>'%s'->SetRealParameter(%d, %s)\n", obj->GetTypeName().c_str(),
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
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            errorMsg2 = " The allowed value is Real number";
         }
         break;
      }
   case Gmat::RMATRIX_TYPE:
      {
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("Setting Rmatrix[%d, %d] from the data"
                  " \"%s\" with ID %d\n", index, colIndex, value.c_str(), id);
         #endif
         Real rval;
         if (GmatStringUtil::ToReal(valueToUse, rval, true))
         {
            obj->SetRealParameter(id, rval, index, colIndex);
         }

         break;
      }
   case Gmat::BOOLEAN_TYPE:
      {
         bool tf;
         if (GmatStringUtil::ToBoolean(valueToUse, tf))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling '%s'->SetBooleanParameter(%d, %d)\n",
                obj->GetName().c_str(), id, tf);
            #endif
            
            obj->SetBooleanParameter(id, tf);
            retval = true;
         }
         else
         {
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            errorMsg2 = " The allowed values are: [true false]";
         }
         break;
      }
   case Gmat::BOOLEANARRAY_TYPE:
      {
         bool tf;
         if (GmatStringUtil::ToBoolean(valueToUse, tf))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetBooleanParameter(%d, %s, %d) (for BooleanArray)\n", 
                obj->GetName().c_str(), id, valueToUse.c_str(), index);
            #endif
            
            retval = obj->SetBooleanParameter(id, tf, index);
         }
         else
         {
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            errorMsg2 = " The allowed values are: [true false]";
         }
         break;
      }
   case Gmat::ON_OFF_TYPE:
      {
         std::string onOff;
         if (GmatStringUtil::ToOnOff(valueToUse, onOff))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling '%s'->SetOnOffParameter(%d, %s)\n",
                obj->GetName().c_str(), id, onOff.c_str());
            #endif
            retval = obj->SetOnOffParameter(id, onOff);
         }
         else
         {
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            errorMsg2 = " The allowed values are case insensitive: [On Off True False]";
         }
         break;
      }
   default:
      InterpreterException ex
         ("Interpreter::SetPropertyValue() Cannot handle the type: " +
          GmatBase::PARAM_TYPE_STRING[type] + " yet.\n");
      HandleError(ex);
      break;
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
      ("Interpreter::SetPropertyObjectValue() obj=<%s> '%s', id=%d, type=%d, value='%s', "
       "index=%d\n", obj->GetTypeName().c_str(), obj->GetName().c_str(), id, type,
       value.c_str(), index);
   #endif
   
   debugMsg = "In SetPropertyObjectValue()";
   Parameter *param = NULL;
   
   // Remove enclosing single quotes first (LOJ: 2009.06.08)
   std::string valueToUse = value;
   valueToUse = GmatStringUtil::RemoveEnclosingString(valueToUse, "'");
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   valueToUse=<%s> after enclosing quote removed\n", valueToUse.c_str());
   #endif
   
   // Try creating Parameter first if it is not ObjectType
   if (!IsObjectType(valueToUse))
   {
      // It is not a one of object types, so create parameter
      param = CreateSystemParameter(valueToUse);
      
      #ifdef DEBUG_SET
      if (param)
         MessageInterface::ShowMessage
            ("   param=<%p><%s>'%s', returnType=%d\n", param,
             param->GetTypeName().c_str(), param->GetName().c_str(), 
             param->GetReturnType());
      #endif
   }
   else
   {
      // It is object type so get parameter (Bug 743 fix)
      param = theModerator->GetParameter(valueToUse);
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("   theModerator->GetParameter() returned %p\n", param);
      #endif
   }
   
   try
   {
      if (param != NULL)
      {
         std::string subMsg;
         // If object is ReportFile or XYPlot, check if Parameter can be reported or plotted
         if ((obj->IsOfType("ReportFile")) && (param->IsSystemParameter() && !param->IsReportable()))
            subMsg = "reportable Parameter";
         else if ((obj->IsOfType("XYPlot")) && (param->IsSystemParameter() && !param->IsPlottable()))
            subMsg = "plottable Parameter";
         if (subMsg != "")
         {
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            errorMsg2 = "  The allowed value is " + subMsg;
            return false;
         }
         
         // Other than Subscriber, it can only take STRING_TYPE parameter
         if (param->GetReturnType() == Gmat::STRING_TYPE ||
             obj->IsOfType(Gmat::SUBSCRIBER))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling '%s'->SetStringParameter(%d, %s)\n",
                obj->GetName().c_str(), id, valueToUse.c_str());
            #endif
            
            // Let base code check for the invalid values
            obj->SetStringParameter(id, valueToUse);
         }
         else
         {
            if (errorMsg1 == "")
               errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
            else
               errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
            //errorMsg2 = ".  The allowed value is valid Object Name";
            return false;
         }
      }
      else
      {
         // check if value is a number
         Real rval;
         Integer ival;
         if (GmatStringUtil::ToReal(valueToUse, rval, true) ||
             GmatStringUtil::ToInteger(valueToUse, ival, true))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   '%s' is a Real or Integer value\n", valueToUse.c_str());
            #endif
            
            // Handle special case for OrbitView.
            // ViewPointReference, ViewPointVector, and ViewDirection can have 
            // both vector and object name.
            if (obj->IsOfType(Gmat::ORBIT_VIEW))
            {
               obj->SetStringParameter(id, valueToUse, index);
            }
            else
            {
               if (errorMsg1 == "")
                  errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
               else
                  errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
               //errorMsg2 = ".  The allowed value is valid Object Name";
               return false;
            }
         }
         
         // check if value is an object name
         GmatBase *configObj = FindObject(valueToUse);
         
         // check if object name is the same as property type name (loj: 2008.11.06)
         // if so, we need to set configObj to NULL so that owned object can be
         // created if needed.
         // ex) Create Propagator RungeKutta89;
         //     GMAT  RungeKutta89.Type = RungeKutta89;
         if (configObj && obj->IsOwnedObject(id))
         {
            ObjectTypeArray refTypes = obj->GetRefObjectTypeArray();
            if (configObj->GetType() != refTypes[id])
               configObj = NULL;
         }
         
         if (configObj)
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Found configured object type of %s\n", configObj->GetTypeName().c_str());
            #endif
            
            bool retval = true;
            
            // Check if object is valid object type
            // (LOJ: 2013.03.18 GMT-3155 FIX)
            Gmat::ObjectType objPropType = obj->GetPropertyObjectType(id);
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   objPropType=%d<%s>\n", objPropType, GmatBase::GetObjectTypeString(objPropType).c_str());
            #endif
            if (objPropType == Gmat::UNKNOWN_OBJECT ||
                (objPropType != Gmat::UNKNOWN_OBJECT && configObj->IsOfType(objPropType)))
            {
               // Set as String parameter, so it can be validated in FinalPass()
               if (index != -1)
               {
                  #ifdef DEBUG_SET
                  MessageInterface::ShowMessage
                     ("   Calling '%s'->SetStringParameter(%d, %s, %d)\n",
                      obj->GetName().c_str(), id, valueToUse.c_str(), index);
                  #endif
                  
                  retval = obj->SetStringParameter(id, valueToUse, index);
               }
               
               // if it has no index or failed setting with index, try without index
               if (index == -1 || !retval)
               {
                  #ifdef DEBUG_SET
                  MessageInterface::ShowMessage
                     ("   Calling '%s'->SetStringParameter(%d, %s)\n",
                      obj->GetName().c_str(), id, valueToUse.c_str());
                  #endif
                  
                  obj->SetStringParameter(id, valueToUse);
               }
            }
            else
            {
               // Ignore error if GetPropertyObjectType() returns Gmat::UNKNOWN_OBJECT.
               // Since not all classes implemented GetPropertyObjectType(), GmatBase
               // returns Gmat::UNKNOWN_OBJECT
               if (objPropType != Gmat::UNKNOWN_OBJECT)
               {
                  std::string toProp = obj->GetParameterText(id);
                  // Handle error message for multiple invalid values
                  if (errorMsg1 == "")
                     errorMsg1 = errorMsg1 + "The value of \"" + value + "\" ";
                  else
                     errorMsg1 = errorMsg1 + "and \"" + value + "\" ";
                  #ifdef DEBUG_SET
                  MessageInterface::ShowMessage
                     ("Interpreter::SetPropertyObjectValue() returning false\n");
                  #endif
                  return false;
               }
            }
         }
         else
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Object not found, so try creating owned object\n");
            #endif
            
            // Create Owned Object, if it is valid owned object type
            GmatBase *ownedObj = NULL;
            bool skipCreate = false;
            if (obj->IsOwnedObject(id))
            {
               // Handle named owned Propagator object for PropSetup
               // since Integrator is not created by Create command
               std::string ownedName = "";
               if (obj->IsOfType(Gmat::PROP_SETUP))
               {
                  ownedName = valueToUse;
                  if (obj->GetParameterText(id) == "FM")
                  {
                     obj->SetStringParameter(id, ownedName);
                     skipCreate = true;
                  }
               }
               if (!skipCreate)
               {
                  ownedObj = CreateObject(valueToUse, ownedName, 0);
                  if (ownedObj == NULL)
                  {
                     // Special case for non-blank invalid axes types (though this should never be called if it's blank)
                     if ((obj->IsOfType("CoordinateSystem")) && (obj->GetParameterText(id) == "Axes") &&
                         (valueToUse != ""))
                     {
                        std::string errmsg = "\"" + valueToUse;
                        errmsg += "\" is an invalid value for field \"Axes\" on coordinate system \"";
                        errmsg += obj->GetName() + "\".\n";
                        throw InterpreterException(errmsg);
                     }
                     else
                     {
                        MessageInterface::ShowMessage
                           ("*** WARNING *** Owned object %s was not created for "
                            "'%s'; using default\n", ownedName.c_str(),
                            obj->GetName().c_str());
                     }
                  }
               }
            }
            
            #ifdef DEBUG_SET
            if (ownedObj)
               MessageInterface::ShowMessage
                  ("   Created ownedObjType: %s\n", ownedObj->GetTypeName().c_str());
            #endif
            
            if (ownedObj)
            {
               // Need to catch the disallowed setting on the built-in coordinate system here
               if ((obj->IsOfType("CoordinateSystem")) && (obj->GetParameterText(id) == "Axes"))
               {
                  obj->SetStringParameter(id, ownedObj->GetTypeName().c_str());
               }
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling '%s'->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
                   ownedObj->GetTypeName().c_str(), ownedObj, ownedObj->GetType());
               #endif
               
               obj->SetRefObject(ownedObj, ownedObj->GetType(), ownedObj->GetName());
               
               // Since PropSetup::SetRefObject() clones Propagator and
               // CoordinateSystem::SetRefObject() clones AxisSystem, delete it from here
               // (LOJ: 2009.03.03)
               if ((obj->GetType() == Gmat::PROP_SETUP &&
                    (ownedObj->GetType() == Gmat::PROPAGATOR)) ||
                   (obj->GetType() == Gmat::COORDINATE_SYSTEM &&
                    (ownedObj->GetType() == Gmat::AXIS_SYSTEM)))
               {
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (ownedObj, ownedObj->GetName(), "Interpreter::SetPropertyObjectValue()",
                      "deleting oldOwnedObject");
                  #endif
                  delete ownedObj;
                  ownedObj = NULL;
               }
            }
            else
            {
               // Special case of InternalODEModel in script
               // Since PropSetup no longer creates InternalODEModel
               // create it here (loj: 2008.11.06)
               if (!skipCreate)
               {
                  if (valueToUse == "InternalODEModel")
                  {
                     ownedObj = CreateObject("ForceModel", valueToUse);
                     obj->SetRefObject(ownedObj, ownedObj->GetType(), valueToUse);
                  }
                  else
                  {
                     // Set as String parameter, so it can be caught in FinalPass()
                     #ifdef DEBUG_SET
                     MessageInterface::ShowMessage
                        ("   Calling '%s'->SetStringParameter(%d, %s) so it can be "
                         "caught in FinalPass()\n",
                         obj->GetName().c_str(), id, valueToUse.c_str());
                     #endif

                     obj->SetStringParameter(id, valueToUse);
                  }
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
         ("Interpreter::SetPropertyObjectValue() Caught BaseException and returning false\n");
      #endif
      
      return false;
   }
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyObjectValue() returning false\n");
   #endif
   return false;     // Function must have return type on all paths
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
   
   debugMsg = "In SetPropertyStringValue()";
   bool retval = true;
   std::string valueToUse = value;
   
   switch (type)
   {
   case Gmat::ENUMERATION_TYPE:
   case Gmat::FILENAME_TYPE:
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
            // remove enclosing quotes if used
            valueToUse = GmatStringUtil::RemoveEnclosingString(valueToUse, "'");
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetStringParameter(%d, %s) (for StringArray)\n",
                obj->GetName().c_str(), id, valueToUse.c_str());
            #endif
            
            retval = obj->SetStringParameter(id, valueToUse);
         }
         catch (BaseException &)
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling %s->SetStringParameter(%d, %s, %d) (for StringArray)\n", 
                     obj->GetName().c_str(), id,
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
   else if (type == Gmat::STRING_TYPE || type == Gmat::ENUMERATION_TYPE ||
            type == Gmat::FILENAME_TYPE)
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
      ("Interpreter::SetProperty() entered, obj=<%p>'%s', id=%d, type=%d, value=<%s>\n",
       obj, obj->GetName().c_str(), id, type, value.c_str());
   #endif
   
   bool retval = false;
   
   std::string valueToUse = value;
   CheckForSpecialCase(obj, id, valueToUse);
   
   // require the object to take its prerequisite action before setting the value
   obj->TakeRequiredAction(id);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("   propertyType=%d, valueToUse=<%s>\n", obj->GetParameterType(id), valueToUse.c_str());
   #endif
   
   StringArray rhsValues;
   Integer count = 0;
   
   // Check if valueToUse is in array form (enclosed with [])
   // LOJ: 2013.03.07 GMT-3564 fix
   if ((type == Gmat::BOOLEANARRAY_TYPE) ||
       (type == Gmat::INTARRAY_TYPE) ||
       (type == Gmat::UNSIGNED_INTARRAY_TYPE) ||
       (type == Gmat::RVECTOR_TYPE))
   {
      if (!GmatStringUtil::IsEnclosedWithBrackets(valueToUse))
      {
         if (errorMsg1 == "")
            errorMsg1 = errorMsg1 + "The value of \"" + valueToUse + "\" ";
         else
            errorMsg1 = errorMsg1 + "and \"" + valueToUse + "\" ";
         errorMsg2 = " Needs [] around values for array type";

         return retval;
      }
   }
   
   // if value has braces or brackets, setting multiple values
   if (value.find("{") != value.npos || value.find("}") != value.npos)
   {
      // verify that we accept only OBJECTARRAYTYPE
      if ((type != Gmat::OBJECTARRAY_TYPE) &&
          (type != Gmat::STRINGARRAY_TYPE))
         return retval;
      // first, check to see if it is a list of strings (e.g. file names);
      // in that case, we do not want to remove spaces inside the strings
      // or use space as a delimiter
      if (value.find("\'") != value.npos)
      {
         std::string trimmed = GmatStringUtil::Trim(value);
         std::string inside  = GmatStringUtil::RemoveOuterString(trimmed, "{", "}");
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage("------> found single quotes in %s\n", value.c_str());
         MessageInterface::ShowMessage("------> trimmed =  %s\n", trimmed.c_str());
         MessageInterface::ShowMessage("------> inside  =  %s\n", inside.c_str());
         #endif
         rhsValues = GmatStringUtil::SeparateByComma(inside);
      }
      else
      {
         rhsValues = theTextParser.SeparateBrackets(value, "{}", " ,");
      }
   }
   else if (value.find("[") != value.npos || value.find("]") != value.npos)
   {
      // verify that we accept only numeric ARRAYTYPE
      // TGG: Short term fix for GMT-3459 GroundTrackPlot.Add allows square brackets
      //      Long term, we need a GetParameterTypeArray or something
	  if (((type != Gmat::BOOLEANARRAY_TYPE) &&
          (type != Gmat::INTARRAY_TYPE) &&
          (type != Gmat::UNSIGNED_INTARRAY_TYPE) &&
          (type != Gmat::RVECTOR_TYPE)) &&
		   (!((type == Gmat::OBJECT_TYPE) && 
		      ((obj->GetParameterText(id) == "ViewDirection") ||
			   (obj->GetParameterText(id) == "ViewPointVector") || 
			   (obj->GetParameterText(id) == "ViewPointReference") ))))
		  return retval;
      // first, check to see if it is a list of strings (e.g. file names);
      // in that case, we do not want to remove spaces inside the strings
      // or use space as a delimiter
      if (value.find("\'") != value.npos)
      {
         std::string trimmed = GmatStringUtil::Trim(value);
         std::string inside = GmatStringUtil::RemoveOuterString(trimmed, "[", "]");
         rhsValues = GmatStringUtil::SeparateByComma(inside);
      }
      else
      {
         rhsValues = theTextParser.SeparateBrackets(value, "[]", " ,");
      }
   }
   
   count = rhsValues.size();
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("   count=%d\n", count);
   #endif
   
   // If rhs value is an array type, call method for setting whole array
   // or call SetPropertValue() with index
   if (count > 0)
   {
      bool setWithIndex = true;
      // See if object has a method to handle whole array
      if (type == Gmat::BOOLEANARRAY_TYPE)
      {
         setWithIndex = false;
         BooleanArray boolArray = GmatStringUtil::ToBooleanArray(value);
         if (boolArray.size() > 0)
         {
            try
            {
               retval = obj->SetBooleanArrayParameter(id, boolArray);
            }
            catch (BaseException &)
            {
               setWithIndex = true;
            }
         }
      }
      
      if (setWithIndex)
      {
         retval = true;
         // Set value with index
         for (int i=0; i<count; i++)
         {
            // If type is RVECTOR_TYPE, we want to catch all non-Real values.
            // Invalid value for other types will be cautht in the FinalPass().
            // (Fix for catching [NONE, 123] for RVECTOR LOJ: 2012.08.10)
            // Actually we want to catch all invalid values, such as non-reportable
            // Parameters, so commented out (LOJ: 2012.11.26)
            //if (type == Gmat::RVECTOR_TYPE)
            //   retval = retval & SetPropertyValue(obj, id, type, rhsValues[i], i);
            //else
               retval = retval & SetPropertyValue(obj, id, type, rhsValues[i], i);
         }
      }
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
   bool retval = true;

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
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("Interpreter::SetComplexProperty() about to set Epoch value %s to spacecraft %s\n",
                     value.c_str(), sc->GetName().c_str());
         #endif
         sc->SetEpoch(value);
      }
      else
      {
         if (parts[0] != "Covariance")
            retval = false;
      }
   }


   if (parts[0] == "Covariance")
   {
      #ifdef DEBUG_SET
         MessageInterface::ShowMessage("Setting covariance elements:\n");
      #endif

      Covariance* covariance = obj->GetCovariance();
      for (UnsignedInt i = 1; i < parts.size(); ++i)
      {
         Integer parmID = obj->GetParameterID(parts[i]);
         Integer covSize = obj->HasParameterCovariances(parmID);
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("   %s, with size %d\n",
                  parts[i].c_str(), covSize);
         #endif

         if (covSize >= 0)
            covariance->AddCovarianceElement(parts[i], obj);
      }

      covariance->ConstructLHS();

      // Check the size of the inputs -- MUST be a square matrix
      StringArray rhsRows;
      if ((value.find("[") == value.npos) || (value.find("]") == value.npos))
         throw GmatBaseException("Covariance matrix definition is missing "
               "square brackets");

      rhsRows = theTextParser.SeparateBrackets(value, "[]", ";");
      UnsignedInt rowCount = rhsRows.size();

      StringArray cells = theTextParser.SeparateSpaces(rhsRows[0]);
      UnsignedInt colCount = cells.size();

      if ((Integer)colCount > covariance->GetDimension())
         throw GmatBaseException("Input covariance matrix is larger than the "
               "matrix built from the input array");

      for (UnsignedInt i = 1; i < rowCount; ++i)
      {
         cells = theTextParser.SeparateSpaces(rhsRows[i]);
       #ifdef DEBUG_SET
            MessageInterface::ShowMessage("   Found  %d columns in row %d\n",
                  cells.size(), i+1);
       #endif

         if (cells.size() != rowCount)
            throw InterpreterException("Row/Column mismatch in the Covariance "
                  "matrix for " + obj->GetName());
      }

      #ifdef DEBUG_SET
         MessageInterface::ShowMessage("Found %d rows and %d columns\n",
               rowCount, colCount);
      #endif

      Integer id = obj->GetParameterID(parts[0]);
      Gmat::ParameterType type = obj->GetParameterType(id);

      for (UnsignedInt i = 0; i < colCount; ++i)
      {
         if (rowCount != 1)
            cells = theTextParser.SeparateSpaces(rhsRows[i]);
         for (UnsignedInt j = 0; j < colCount; ++j)
            if (i == j)
               SetPropertyValue(obj, id, type, cells[j], i, j);
            else
               // If a single row, it's the diagonal
               if (rowCount == 1)
                  SetPropertyValue(obj, id, type, "0.0", i, j);
               // Otherwise it's cell[j]
               else
                  SetPropertyValue(obj, id, type, cells[j], i, j);
      }

      #ifdef DEBUG_SET
         MessageInterface::ShowMessage("Covariance matrix set to:\n");
         for (UnsignedInt i = 0; i < colCount; ++i)
         {
            MessageInterface::ShowMessage("   [");
            for (UnsignedInt j = 0; j < colCount; ++j)
               MessageInterface::ShowMessage(" %.12lf ", (*obj->GetCovariance())(i,j));
            MessageInterface::ShowMessage("]\n");
         }
      #endif
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetForceModelProperty(GmatBase *obj, const std::string &prop,
//                            const std::string &value, GmatBase *fromObj)
//------------------------------------------------------------------------------
/**
 * Configures properties for an ODEModel
 *
 * ODEModel configuration is performed through calls to this method.  The method
 * sets general ODEModel parameters, and includes constructor calls for the
 * PhysicalModels that constitute the contributors, through superposition, to
 * the total derivative data at a given state.
 *
 * @param obj     The ODEModel that receives the setting
 * @param prop    The property that is being set
 * @param value   The value for the property
 * @param fromObj Object supplying the value, if used
 *
 * @return true if the property is set, false if not set
 */
//------------------------------------------------------------------------------
bool Interpreter::SetForceModelProperty(GmatBase *obj, const std::string &prop,
                                  const std::string &value, GmatBase *fromObj)
{
   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("Interpreter::SetForceModelProperty() entered, obj=<%p><%s>'%s', prop=<%s>, "
       "value=<%s>, fromObj=<%p>\n", obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
       prop.c_str(), value.c_str(), fromObj);
   #endif
   
   debugMsg = "In SetForceModelProperty()";
   bool retval = false;
   bool pmTypeHandled = false;

   StringArray parts = theTextParser.SeparateDots(prop);

   #ifdef DEBUG_SET_FORCE_MODEL
      MessageInterface::ShowMessage("   %s decomposes to", prop.c_str());
      for (UnsignedInt i = 0; i < parts.size(); ++i)
         MessageInterface::ShowMessage(" [%s]", parts[i].c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   Integer dotCount = parts.size();
   std::string pmType = parts[dotCount-1];
   Integer id;
   Gmat::ParameterType type;
   
   ODEModel *forceModel = (ODEModel*)obj;
   std::string forceType = ODEModel::GetScriptAlias(pmType);
   std::string centralBodyName = forceModel->GetStringParameter("CentralBody");
   
   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("   pmType=%s, forceType=%s\n", pmType.c_str(), forceType.c_str());
   #endif
   
   //------------------------------------------------------------
   // Set ForceModel CentralBody
   //------------------------------------------------------------
   if (pmType == "CentralBody")
   {
      id = obj->GetParameterID("CentralBody");
      type = obj->GetParameterType(id);
      retval = SetPropertyValue(obj, id, type, value);
      #ifdef DEBUG_SET_FORCE_MODEL
         MessageInterface::ShowMessage("Interpreter::SetForceModelProperty() "
               "set central body and returning %s\n",
               retval ? "true" : "false");
      #endif
      return retval;
   }
   
   //------------------------------------------------------------
   // Create ForceModel owned PhysicalModel
   //------------------------------------------------------------
   
   else if (pmType == "PrimaryBodies" || pmType == "PointMasses" ||
            pmType == "PolyhedralBodies")
   {
      retval = true;
      StringArray bodies = theTextParser.SeparateBrackets(value, "{}", " ,");
      
      // Clear the old point mass forces if a new list is found
      if (pmType == "PointMasses")
         forceModel->TakeAction("ClearForcesOfType", "PointMassForce");

      for (UnsignedInt i = 0; i < bodies.size(); ++i)
      {
         #ifdef DEBUG_SET_FORCE_MODEL
         MessageInterface::ShowMessage("   bodies[%d]=%s\n", i, bodies[i].c_str());
         #endif
         
         // We don't want to configure PhysicalModel, so set name after create
         ////PhysicalModel *pm = (PhysicalModel*)CreateObject(forceType, "");
         std::string forceName = forceType + "." + bodies[i];
         PhysicalModel *pm = (PhysicalModel*)CreateObject(forceType, "0."+forceName, 0);
         if (pm)
         {
            ////pm->SetName(forceType + "." + bodies[i]);
            pm->SetName(forceName);
            
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
            
            // Since default GravityField is created when ForceModel is created
            // We need to empty the ForceModel before adding new force. (LOJ: 2010.09.01)
            // Add force to ForceModel
            forceModel->TakeAction("ClearDefaultForce");
            forceModel->AddForce(pm);
            
            #ifdef DEBUG_SET_FORCE_MODEL
               MessageInterface::ShowMessage
                  ("   ... %s has been added to ForceModel %s\n",
                   pm->GetName().c_str(), forceModel->GetName().c_str());
            #endif

            // Use JGM2 for default Earth gravity file, in case it is not
            // specified in the script
            if (pmType == "PrimaryBodies" && bodies[i] == "Earth")
            {
               id = pm->GetParameterID("Model");
               type = pm->GetParameterType(id);
               retval = SetPropertyValue(pm, id, type, "JGM2");
            }
            if (pmType == "PrimaryBodies" && bodies[i] == "Luna")
            {
               id = pm->GetParameterID("Model");
               type = pm->GetParameterType(id);
               retval = SetPropertyValue(pm, id, type, "LP165P");
            }
            if (pmType == "PrimaryBodies" && bodies[i] == "Venus")
            {
               id = pm->GetParameterID("Model");
               type = pm->GetParameterType(id);
               retval = SetPropertyValue(pm, id, type, "MGNP180U");
            }
            if (pmType == "PrimaryBodies" && bodies[i] == "Mars")
            {
               id = pm->GetParameterID("Model");
               type = pm->GetParameterType(id);
               retval = SetPropertyValue(pm, id, type, "MARS50C");
            }
         }
      }
      
      #ifdef DEBUG_SET_FORCE_MODEL
      MessageInterface::ShowMessage
         ("Interpreter::SetForceModelProperty() returning %s\n",
               retval ? "true" : "false");
      #endif
      return retval;
   }
   else if (pmType == "Drag" || pmType == "AtmosphereModel")
   {
      // Write deprecated message, now we only use Drag.AtmosphereModel to specify model name
      if (pmType == "Drag" && value != "None")
      {
         InterpreterException ex
            ("The field \"Drag\" of ForceModel \"" + obj->GetName() +
             "\" will not be permitted in a future build; "
             "please use \"Drag.AtmosphereModel\" instead");
         HandleError(ex, true, true);
      }
      
      // If value is None, do not create DragForce
      if (value == "None")
         return true;
      
      // Special handling for Drag
      // If field is AtmosphereModel, create DragForce and then AtmosphereModel.
      // It will also handle old script such as FM.Drag = JacchiaRoberts
      return SetDragForceProperty(obj, "Drag", pmType, value);
   }
   else if (pmType == "SRP" || pmType == "RelativisticCorrection")
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
      
      if (pmType == "RelativisticCorrection")
      {
         id = obj->GetParameterID("RelativisticCorrection");
         type = obj->GetParameterType(id);
         retval = SetPropertyValue(obj, id, type, value);

         if (retval && value != "On")
            return true;
         else if (!retval)
            return false;
      }
      
      // Create PhysicalModel
      std::string forceName = pmType + "." + centralBodyName;
      //@note 0.ForceName indicates unmanaged internal forcename.
      // Added name for debugging purpose only
      PhysicalModel *pm = (PhysicalModel*)CreateObject(forceType, "0."+forceName, 0);
      pm->SetName(forceName);
      
      // Should we set SRP on ForceModel central body?
      pm->SetStringParameter("BodyName", centralBodyName);
      
      #ifdef DEBUG_SET_FORCE_MODEL
      MessageInterface::ShowMessage
         ("   Adding PhysicalModel <%p><%s>'%s' to ForceModel:<%s>\n", pm,
          pm->GetTypeName().c_str(), pm->GetName().c_str(),
          forceModel->GetName().c_str());
      #endif
      
      // Add force to ForceModel
      forceModel->AddForce(pm);
      
      #ifdef DEBUG_SET_FORCE_MODEL
      MessageInterface::ShowMessage("Interpreter::SetForceModelProperty() "
            "returning true\n");
      #endif
      return true;
   }
   // User defined forces
   else if (pmType == "UserDefined")
   {
      StringArray udForces = theTextParser.SeparateBrackets(value, "{}", " ,");
      
      for (UnsignedInt i=0; i<udForces.size(); i++)
      {
         #ifdef DEBUG_SET_FORCE_MODEL
            MessageInterface::ShowMessage("   User defined force[%d] = %s\n", 
                  i, udForces[i].c_str());
         #endif
         
         // We don't want to configure PhysicalModel, so set name after create
         ////PhysicalModel *pm = (PhysicalModel*)CreateObject(udForces[i], "");
         PhysicalModel *pm = (PhysicalModel*)CreateObject(udForces[i], udForces[i], 0);
         if (pm)
         {
            pm->SetName(udForces[i]);
            forceModel->AddForce(pm);
         }
         else
            throw InterpreterException
               ("User defined force \"" + udForces[i] +  "\" cannot be created\n");
      }

      pmTypeHandled = true;
      // Should we return here?
   }
   
   //------------------------------------------------------------
   // Set ForceModel owned object properties
   //------------------------------------------------------------
   
   pmType = parts[0];
   forceType = ODEModel::GetScriptAlias(pmType);
   std::string propName = parts[dotCount-1];
   std::string qualifier = "";
   
   if (parts.size() == 3)
	   qualifier = parts[1];

   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("   Setting pmType=%s, forceType=%s, propName=%s\n", pmType.c_str(),
       forceType.c_str(), propName.c_str());
   if (qualifier != "")
	   MessageInterface::ShowMessage("   Parameter has the qualifier %s\n",
			   qualifier.c_str());
   #endif
   
   GmatBase *owner;
   Integer propId;
   Gmat::ParameterType propType;

   if (FindPropertyID(forceModel, propName, &owner, propId, propType))
   {
      id = owner->GetParameterID(propName);
      type = owner->GetParameterType(id);

      // If there is a qualifier, it should match the model qualifier
      if (qualifier != "")
      {

         std::stringstream errmsg;

         if (pmType == "Drag")
         {
            std::string atModel = owner->GetStringParameter("AtmosphereModel");
            if (atModel != qualifier)
               errmsg << "The atmosphere model type \"" << qualifier
                      << "\" does not match the current " << atModel
                      << " drag model type.  In addition, the ";
            else
               errmsg << "The ";

            errmsg << "behavior of the field \"" << prop
                   << "\" in Forcemodel \"" << obj->GetName()
                   << "\" has been deprecated and will not be supported in "
                   << "future versions. Please use the syntax \""
                   << "Drag." << propName << "\"";

            InterpreterException ex(errmsg.str());
            HandleError(ex, true, true);
         }
      }

      // Ensure that the qualifier is correct for the model
      if (owner->IsOfType(Gmat::PHYSICAL_MODEL) && (qualifier != ""))
      {
    	   if (((PhysicalModel*)owner)->CheckQualifier(qualifier, forceType) == false)
    	   {
    	      throw InterpreterException("The property \"" + prop +
    	               "\" cannot be set in the ODE Model \"" + obj->GetName() +
    	               "\"");
         }
      }

      retval = SetPropertyValue(owner, id, type, value);
      if (fromObj != NULL)
         owner->SetRefObject(fromObj, fromObj->GetType(), value);
      pmTypeHandled = true;
   }
   else
   {
      // Try owned object from ODEModel
      for (int i = 0; i < forceModel->GetOwnedObjectCount(); i++)
      {
         GmatBase *ownedObj = forceModel->GetOwnedObject(i);
         if (ownedObj && FindPropertyID(ownedObj, propName, &owner, propId, propType))
         {
            id = owner->GetParameterID(propName);
            type = owner->GetParameterType(id);
            bool rv = SetPropertyValue(owner, id, type, value);

            if (rv)
            {
               retval = true;
               pmTypeHandled = true;
            }

            break;
         }
      }
   }
   
   if (pmTypeHandled == false)
      throw InterpreterException("The scripted force type \"" + pmType +
            "\" is not a known force or force model setting\n");

   #ifdef DEBUG_SET_FORCE_MODEL
      MessageInterface::ShowMessage
         ("Interpreter::SetForceModelProperty() exiting and returning %s\n",
               retval ? "true" : "false");
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool SetDragForceProperty(GmatBase *obj, const std::string &pmType, ...)
//------------------------------------------------------------------------------
/**
 * Creates DragForce and AtmosphereModel objects and adds to ODEModel.
 *
 * @param  obj  ODEModel object
 * @param  pmType  First field string (This is not the actual PhysicalModel type name)
 */
//------------------------------------------------------------------------------
bool Interpreter::SetDragForceProperty(GmatBase *obj,
                                       const std::string &pmType,
                                       const std::string &propName,
                                       const std::string &value)
{
   ODEModel *forceModel = (ODEModel*)obj;
   std::string forceType = ODEModel::GetScriptAlias(pmType);
   std::string centralBodyName = forceModel->GetStringParameter("CentralBody");
   std::string valueToUse = GmatStringUtil::RemoveEnclosingString(value, "'");
   
   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("Interpreter::SetDragForceProperty() entered, forceType=<%s>, pmType=<%s>, "
       "propName=<%s>, value=<%s>, centralBodyName=<%s>\n", forceType.c_str(), pmType.c_str(),
       propName.c_str(), value.c_str(), centralBodyName.c_str());
   #endif
   
   // Create DragForce
   //@note 0.ForceName indicates unmanaged internal forcename.
   // Added name for debugging purpose only
   std::string forceName = pmType + "." + centralBodyName;
   PhysicalModel *pm = (PhysicalModel*)CreateObject(forceType, "0."+forceName, 0);
   pm->SetName(forceName);
   
   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("   PhysicalModel <%p><%s>'%s' created\n", pm, pm->GetTypeName().c_str(),
       pm->GetName().c_str());
   #endif
   
   if (!pm->SetStringParameter("AtmosphereModel", valueToUse))
   {
      InterpreterException ex
         ("Unable to set AtmosphereModel " + valueToUse + " for drag force");
      HandleError(ex);
      ignoreError = true;
      return false;
   }
   
   // Create AtmosphereModel for the primary body
   if (valueToUse != "BodyDefault")
   {
      #ifdef DEBUG_SET_FORCE_MODEL
      MessageInterface::ShowMessage
         ("   Creating AtmosphereModel of type '%s'\n", valueToUse.c_str());
      #endif
      
      pm->SetStringParameter("BodyName", centralBodyName);
      pm->SetStringParameter("AtmosphereBody", centralBodyName);
      GmatBase *am = CreateObject(valueToUse, valueToUse, 0);
      if (am)
      {
         pm->SetRefObject(am, Gmat::ATMOSPHERE, am->GetName());
      }
      else
      {
         InterpreterException ex
            ("Unable to create AtmosphereModel \"" + valueToUse + "\" for drag force");
         HandleError(ex);
         ignoreError = true;
         return false;
      }
   }
   
   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("   Adding PhysicalModel <%p><%s>'%s' to ForceModel:<%s>\n", pm,
       pm->GetTypeName().c_str(), pm->GetName().c_str(),
       forceModel->GetName().c_str());
   #endif
   
   // Add force to ForceModel
   forceModel->AddForce(pm);
   
   #ifdef DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage("Interpreter::SetDragForceProperty() returning true\n");
   #endif
   return true;
}



//------------------------------------------------------------------------------
// bool Interpreter::SetMeasurementModelProperty(GmatBase *obj,
//          const std::string &property, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method configures properties on a MeasurementModel
 *
 * The method creates CoreMeasurements as needed, and passes the remaining
 * parameters to the SetProperty() method.
 *
 * @param obj        The MeasurementModel that is being configured
 * @param property   The property that is being set
 * @param value      The property's value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Interpreter::SetMeasurementModelProperty(GmatBase *obj,
         const std::string &property, const std::string &value)
{
   debugMsg = "In SetMeasurementModelProperty()";
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(property);
   Integer count = parts.size();
   std::string propName = parts[count-1];

   #ifdef DEBUG_SET_MEASUREMENT_MODEL
      MessageInterface::ShowMessage
         ("Interpreter::SetMeasurementModelProperty() mModel=%s, prop=%s, "
          "value=%s\n", obj->GetName().c_str(), propName.c_str(),
          value.c_str());
   #endif

   if (propName == "Type")
   {
      GmatBase* model = CreateObject(value, "", 0, false);
      if (model != NULL)
      {
         if (model->IsOfType(Gmat::CORE_MEASUREMENT))
            retval = obj->SetRefObject(model, Gmat::CORE_MEASUREMENT, "");
      }
      else
         throw InterpreterException("Failed to create a " + value +
               " core measurement");
   }
   else
   {
      Integer id;
      Gmat::ParameterType type;

      StringArray parts = theTextParser.SeparateDots(property);
      // if property has multiple dots, handle separately
      if (parts.size() > 1)
      {
         retval = SetComplexProperty(obj, property, value);
         if (retval)
            return retval;
      }

      id = obj->GetParameterID(property);
      type = obj->GetParameterType(id);
      if (property == "Covariance")
      {
         // Check the size of the inputs -- MUST be a square matrix
         StringArray rhsRows;
         if ((value.find("[") == value.npos) || (value.find("]") == value.npos))
            throw GmatBaseException("Covariance matrix definition is missing "
                  "square brackets");

         rhsRows = theTextParser.SeparateBrackets(value, "[]", ";");
         UnsignedInt rowCount = rhsRows.size();

         StringArray cells = theTextParser.SeparateSpaces(rhsRows[0]);
         UnsignedInt colCount = cells.size();

         Covariance *covariance = obj->GetCovariance();

         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("%s covariance has dim %d, "
                  "row count = %d, colCount = %d\n", obj->GetName().c_str(),
                  covariance->GetDimension(), rowCount, colCount);
         #endif

         if ((Integer)colCount > covariance->GetDimension())
            throw GmatBaseException("Input covariance matrix is larger than the "
                  "matrix built from the input array");

         for (UnsignedInt i = 1; i < rowCount; ++i)
         {
            cells = theTextParser.SeparateSpaces(rhsRows[i]);
          #ifdef DEBUG_SET
               MessageInterface::ShowMessage("   Found  %d columns in row %d\n",
                     cells.size(), i+1);
          #endif

            if (cells.size() != rowCount)
               throw InterpreterException("Row/Column mismatch in the Covariance "
                     "matrix for " + obj->GetName());
         }

         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("Found %d rows and %d columns\n",
                  rowCount, colCount);
         #endif

         for (UnsignedInt i = 0; i < colCount; ++i)
         {
            if (rowCount != 1)
               cells = theTextParser.SeparateSpaces(rhsRows[i]);
            for (UnsignedInt j = 0; j < colCount; ++j)
               if (i == j)
                  SetPropertyValue(obj, id, type, cells[j], i, j);
               else
                  // If a single row, it's the diagonal
                  if (rowCount == 1)
                     SetPropertyValue(obj, id, type, "0.0", i, j);
                  // Otherwise it's cell[j]
                  else
                     SetPropertyValue(obj, id, type, cells[j], i, j);
         }

         retval = true;
      }
      else
         retval = SetProperty(obj, id, type, value);
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool SetTrackingDataProperty(GmatBase *obj, const std::string &prop,
//                              const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method...
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
bool Interpreter::SetTrackingDataProperty(GmatBase *obj,
         const std::string &property, const std::string &value)
{
   debugMsg = "In SetTrackingDataProperty()";
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(property);
   Integer count = parts.size();
   std::string propName = parts[count-1];

   #ifdef DEBUG_SET_MEASUREMENT_MODEL
      MessageInterface::ShowMessage
         ("Interpreter::SetTrackingDataProperty() mModel=%s, prop=%s, "
          "value=%s\n", obj->GetName().c_str(), propName.c_str(),
          value.c_str());
   #endif

   if (propName == "Type")
   {
      GmatBase* model = CreateObject(value, "", 0, false);
      if (model != NULL)
      {
         if (model->IsOfType(Gmat::CORE_MEASUREMENT))
            retval = obj->SetRefObject(model, Gmat::CORE_MEASUREMENT, "");
      }
      else
         throw InterpreterException("Failed to create a " + value +
               " core measurement");
   }
   else
   {
      Integer id;
      Gmat::ParameterType type;

      StringArray parts = theTextParser.SeparateDots(property);
      // if property has multiple dots, handle separately
      if (parts.size() > 1)
      {
         retval = SetComplexProperty(obj, property, value);
         if (retval)
            return retval;
      }

      id = obj->GetParameterID(property);
      type = obj->GetParameterType(id);
      if (property == "Covariance")
      {
         // Check the size of the inputs -- MUST be a square matrix
         StringArray rhsRows;
         if ((value.find("[") == value.npos) || (value.find("]") == value.npos))
            throw GmatBaseException("Covariance matrix definition is missing "
                  "square brackets");

         rhsRows = theTextParser.SeparateBrackets(value, "[]", ";");
         UnsignedInt rowCount = rhsRows.size();

         StringArray cells = theTextParser.SeparateSpaces(rhsRows[0]);
         UnsignedInt colCount = cells.size();

         Covariance *covariance = obj->GetCovariance();

         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("%s covariance has dim %d, "
                  "row count = %d, colCount = %d\n", obj->GetName().c_str(),
                  covariance->GetDimension(), rowCount, colCount);
         #endif

         if ((Integer)colCount > covariance->GetDimension())
            throw GmatBaseException("Input covariance matrix is larger than the "
                  "matrix built from the input array");

         for (UnsignedInt i = 1; i < rowCount; ++i)
         {
            cells = theTextParser.SeparateSpaces(rhsRows[i]);
          #ifdef DEBUG_SET
               MessageInterface::ShowMessage("   Found  %d columns in row %d\n",
                     cells.size(), i+1);
          #endif

            if (cells.size() != rowCount)
               throw InterpreterException("Row/Column mismatch in the Covariance "
                     "matrix for " + obj->GetName());
         }

         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("Found %d rows and %d columns\n",
                  rowCount, colCount);
         #endif

         for (UnsignedInt i = 0; i < colCount; ++i)
         {
            if (rowCount != 1)
               cells = theTextParser.SeparateSpaces(rhsRows[i]);
            for (UnsignedInt j = 0; j < colCount; ++j)
               if (i == j)
                  SetPropertyValue(obj, id, type, cells[j], i, j);
               else
                  // If a single row, it's the diagonal
                  if (rowCount == 1)
                     SetPropertyValue(obj, id, type, "0.0", i, j);
                  // Otherwise it's cell[j]
                  else
                     SetPropertyValue(obj, id, type, cells[j], i, j);
         }

         retval = true;
      }
      else
         retval = SetProperty(obj, id, type, value);
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetTrackingSystemProperty(GmatBase *obj, const std::string &prop,
//          const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method...
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
bool Interpreter::SetTrackingSystemProperty(GmatBase *obj,
         const std::string &prop, const std::string &value)
{
   debugMsg = "In SetTrackingSystemProperty()";
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(prop);

   // if property has multiple dots, handle separately
   if (parts.size() > 1)
   {
      retval = SetComplexProperty(obj, prop, value);
      if (retval)
         return retval;
   }

   Integer id;
   Gmat::ParameterType type;
   id = obj->GetParameterID(prop);
   type = obj->GetParameterType(id);
   retval = SetProperty(obj, id, type, value);
   return retval;
}


//------------------------------------------------------------------------------
// bool Interpreter::SetDataStreamProperty(GmatBase *obj,
//          const std::string &property, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method configures properties on a DataStream
 *
 * The method creates ObTypes as needed, and passes the remaining
 * parameters to the SetProperty() method.
 *
 * @param obj        The DataStream that is being configured
 * @param property   The property that is being set
 * @param value      The property's value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Interpreter::SetDataStreamProperty(GmatBase *obj,
         const std::string &property, const std::string &value)
{
   debugMsg = "In SetDataStreamProperty()";
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(property);
   Integer count = parts.size();
   std::string propName = parts[count-1];

   #ifdef DEBUG_SET_MEASUREMENT_MODEL
      MessageInterface::ShowMessage
         ("Interpreter::SetDataStreamProperty() mModel=%s, prop=%s, "
          "value=%s\n", obj->GetName().c_str(), propName.c_str(),
          value.c_str());
   #endif

   if (propName == "Format")
   {
      GmatBase* obs = CreateObject(value, "", 0, false);
      if (obs != NULL)
      {
         if (obs->IsOfType(Gmat::OBTYPE))
            retval = obj->SetRefObject(obs, Gmat::OBTYPE);
      }
      else
         throw InterpreterException("Failed to create a " + value +
               " observation type");
   }
   else
   {
      Integer id;
      Gmat::ParameterType type;

      id = obj->GetParameterID(property);
      type = obj->GetParameterType(id);
      retval = SetProperty(obj, id, type, value);
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
   
   debugMsg = "In SetSolarSystemProperty()";
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(prop);
   Integer count = parts.size();
   SolarSystem *solarSystem = (SolarSystem *)obj;
   
   if (count == 1)
   {
//      if (prop == "Ephemeris")
//      {
//         StringArray ephems = theTextParser.SeparateBrackets(value, "{}", " ,");
//      
//         #ifdef DEBUG_SET_SOLAR_SYS
//         for (StringArray::iterator i = ephems.begin(); i != ephems.end(); ++i)
//            MessageInterface::ShowMessage("   Source = %s\n", i->c_str());
//         #endif
//         
//         theModerator->SetPlanetarySourceTypesInUse(ephems);
//         retval = true;
//      }
//      else
//      {
         Integer id = obj->GetParameterID(prop);
         Gmat::ParameterType type = obj->GetParameterType(id);
         retval = SetPropertyValue(obj, id, type, value);
//      }
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
         #ifdef DEBUG_SET_SOLAR_SYS
         MessageInterface::ShowMessage("SolarSystemException thrown: %s\n", e.GetFullMessage().c_str());
         #endif
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
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::FindOwnedObject() owner=<%s>, toProp=<%s>\n",
       owner->GetName().c_str(), toProp.c_str());
   #endif
   
   debugMsg = "In FindOwnedObject()";
   bool retval = false;
   Integer ownedObjCount = owner->GetOwnedObjectCount();
   Integer errorCount = 0;
   GmatBase *tempObj = NULL;
   *ownedObj = NULL;
   
   // Initialize output parameters
   id = -1;
   type = Gmat::UNKNOWN_PARAMETER_TYPE;
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage("   ownedObjCount=%d\n", ownedObjCount);
   #endif
   
   if (ownedObjCount > 0)
   {
      for (int i=0; i<ownedObjCount; i++)
      {
         tempObj = owner->GetOwnedObject(i);
         if (tempObj)
         {
            #ifdef DEBUG_FIND_OBJECT
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
            catch (BaseException &)
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
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("   FindOwnedObject() returning retval=%d, ownedObj=<%p>\n", retval, *ownedObj);
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
   
   debugMsg = "In GetArrayValue()";
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


//------------------------------------------------------------------------------
// bool ParseVariableExpression(Parameter *var, const std::string &exp)
//------------------------------------------------------------------------------
bool Interpreter::ParseVariableExpression(Parameter *var, const std::string &exp)
{
   if (var == NULL)
   {
      InterpreterException ex
         ("Interpreter::ParseVariableExpression() The variable is NULL\n");
      HandleError(ex);
      return false;
   }
   
   #ifdef DEBUG_VAR_EXPRESSION
   MessageInterface::ShowMessage
      ("Interpreter::ParseVariableExpression() entered, var=<%p>'%s', exp='%s'\n",
       var, var->GetName().c_str(), exp.c_str());
   #endif
   
   // Check for invalid starting name such as 1(x) should give an error (loj: 2008.08.15)
   if (exp.find_first_of("(") != exp.npos)
   {
      if (!GmatStringUtil::IsValidName(exp, true))
      {
         #ifdef DEBUG_VAR_EXPRESSION
         MessageInterface::ShowMessage
            ("Interpreter::ParseVariableExpression() returning false, '%s' is not "
             "a valid name\n", exp.c_str());
         #endif
         return false;
      }
   }
   
   // Parse the Parameter
   StringTokenizer st(exp, "()*/+-^ ");
   StringArray tokens = st.GetAllTokens();
   Real rval;
   
   // Check if non-existing variables used in expression
   for (unsigned int i=0; i<tokens.size(); i++)
   {
      #ifdef DEBUG_VAR_EXPRESSION
      MessageInterface::ShowMessage("   token:<%s> \n", tokens[i].c_str());
      #endif
      
      if (!GmatStringUtil::ToReal(tokens[i], rval))
      {
         #ifdef DEBUG_VAR_EXPRESSION
         MessageInterface::ShowMessage
            ("   It is not a number, so trying to create a Parameter\n");
         #endif
         
         Parameter *param = CreateSystemParameter(tokens[i]);
         if (param)
         {
            #ifdef DEBUG_VAR_EXPRESSION
            MessageInterface::ShowMessage
               ("   The Parameter '%s' found or created, so setting it to '%s' "
                "as ref object name\n",  param->GetName().c_str(), var->GetName().c_str());
            #endif
            // set parameter names used in expression
            var->SetRefObjectName(Gmat::PARAMETER, tokens[i]);
         }
         else
         {
            #ifdef DEBUG_VAR_EXPRESSION
            MessageInterface::ShowMessage
               ("Interpreter::ParseVariableExpression() returning false "
                "since '%s' is not allowed in the expression\n", tokens[i].c_str());
            #endif
            
            //InterpreterException ex
            //   ("The Variable \"" + tokens[i] + "\" does not exist. "
            //    "It must be created first");
            //HandleError(ex);
            return false;
         }
      }
   }
   
   var->SetStringParameter("Expression", exp);
   
   return true;
}


//------------------------------------------------------------------------------
// AxisSystem* CreateAxisSystem(std::string type, GmatBase *owner)
//------------------------------------------------------------------------------
AxisSystem* Interpreter::CreateAxisSystem(std::string type, GmatBase *owner)
{
   #ifdef DEBUG_AXIS_SYSTEM
   MessageInterface::ShowMessage
      ("Interpreter::CreateAxisSystem() type = '%s'\n", type.c_str());
   #endif
   
   AxisSystem *axis = theValidator->CreateAxisSystem(type, owner);
   
   // Handle error messages here
   if (axis == NULL)
   {
      StringArray errList = theValidator->GetErrorList();
      for (UnsignedInt i=0; i<errList.size(); i++)
         HandleError(InterpreterException(errList[i]));
   }
   
   #ifdef DEBUG_AXIS_SYSTEM
   MessageInterface::ShowMessage
      ("Interpreter::CreateAxisSystem() returning <%p>\n", axis);
   #endif
   
   return axis;
}


//------------------------------------------------------------------------------
// void HandleError(const BaseException &e, bool writeLine, bool warning ...)
//------------------------------------------------------------------------------
void Interpreter::HandleError(const BaseException &e, bool writeLine, bool warning)
{
   if (writeLine)
   {
      Integer lineNum = theReadWriter->GetLineNumber();
      if (inScriptEvent)
         lineNum = lineNum - 1;

      #ifdef DEBUG_HANDLE_ERROR
      MessageInterface::ShowMessage
         ("Interpreter::HandleError(), inScriptEvent=%d, lineNum=%d\n",
          inScriptEvent, lineNum);
      #endif
      
      lineNumber = GmatStringUtil::ToString(lineNum);
      currentLine = theReadWriter->GetCurrentLine();
      
      HandleErrorMessage(e, lineNumber, currentLine, writeLine, warning);
   }
   else
   {
      HandleErrorMessage(e, "", "", writeLine, warning);
   }
}


//------------------------------------------------------------------------------
// void HandleErrorMessage(const BaseException &e, const std::string &lineNumber...)
//------------------------------------------------------------------------------
void Interpreter::HandleErrorMessage(const BaseException &e,
                                     const std::string &lineNumber,
                                     const std::string &line,
                                     bool writeLine, bool warning)
{
   std::string currMsg = "";
   std::string msgKind = "**** ERROR **** ";
   if (warning)
      msgKind = "*** WARNING *** ";
   
   // Added function name in the message (loj: 2008.08.29)
   std::string fnMsg;
   if (currentFunction != NULL)
   {
      fnMsg = currentFunction->GetFunctionPathAndName();
      fnMsg = "(In Function \"" + fnMsg + "\")\n";
      if (!writeLine)
         fnMsg = "\n" + fnMsg;
   }
   
   if (writeLine)
      currMsg = " in line:\n" + fnMsg + "   \"" + lineNumber + ": " + line + "\"\n";
   else
      currMsg = fnMsg;
   
   std::string msg = msgKind + e.GetFullMessage() + currMsg;
   
   #ifdef DEBUG_HANDLE_ERROR
   MessageInterface::ShowMessage("%s, continueOnError=%d\n", debugMsg.c_str(), continueOnError);
   #endif
   
   if (continueOnError)
   {
      // remove duplicate exception message
      msg = GmatStringUtil::Replace(msg, "Interpreter Exception: Interpreter Exception: ",
                                    "Interpreter Exception: ");
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
      {
         // remove duplicate exception message
         msg = GmatStringUtil::Replace(msg, "**** ERROR **** Interpreter Exception: ", "");
         throw InterpreterException(msg);
      }
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
       parts[0] == "BeginScript" || parts[0] == "EndScript" ||
       parts[0] == "Else")
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
   
   // Check for unbalanced branch commands
   
   debugMsg = "In CheckBranchCommands()";
   std::stack<std::string> controlStack;
   std::string expEndStr, str, str1;
   bool retval = true;
   bool elseIsOk = true;
   
   #ifdef DEBUG_CHECK_BRANCH
   MessageInterface::ShowMessage("   Now start checking %d lines\n",
         lines.size());
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
            InterpreterException ex;
            if ((str1 == "If") && (elseIsOk))
            {
               ex.SetDetails(
                  "Expecting \"Else\" or \"EndIf\" but found \"" + str + "\"");
            }
            else
            {
               ex.SetDetails(
                  "Expecting \"" + expEndStr + "\" but found \"" + str + "\"");
            }
            HandleErrorMessage(ex, GmatStringUtil::ToString(lineNumbers[i]), str);
            retval = false;
            break;
         }
         if (str1 == "If")
            elseIsOk = true;  // Else is OK to find in the next If/EndIf
      }
      else if (GmatStringUtil::StartsWith(str, "Else"))
      {
         if (controlStack.empty())
         {
            InterpreterException ex("Found \"Else\" without corresponding \"If\"");
            HandleErrorMessage(ex, GmatStringUtil::ToString(lineNumbers[i]), str);
            retval = false;
            break;
         }

         str1 = controlStack.top();
         // don't pop If off because we still need it for checking against the EndIf
         if (str1 != "If")
         {
            controlStack.pop();
         }
         else
         {
            elseIsOk = false;
         }

         if (str1 == "BeginScript")
            expEndStr = "EndScript";
         else
            expEndStr = "End" + str1;

         if (expEndStr != "EndIf")
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
         if (controlStack.top() == "BeginScript")
            expEndStr = "EndScript";
         else
            expEndStr = "End" + controlStack.top();

         InterpreterException ex
            ("Matching \"" + expEndStr + "\" not found for \"" +
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
 * @note: Most objects have reference objects already set in the SetObject*(),
 *        if parameter type is OBJECT_TYPE, so not requiring additional call to
 *        SetRefObject()
 */
//------------------------------------------------------------------------------
bool Interpreter::FinalPass()
{
   #if DBGLVL_FINAL_PASS
   MessageInterface::ShowMessage("Interpreter::FinalPass() entered\n");
   #endif
   
   debugMsg = "In FinalPass()";
   bool retval = true;
   GmatBase *obj = NULL;
   GmatBase *refObj;
   StringArray refNameList;
   std::string objName;
   StringArray objList;
   
   objList = theModerator->GetListOfObjects(Gmat::UNKNOWN_OBJECT);
   SolarSystem *ss = theModerator->GetSolarSystemInUse();
   objList.push_back(ss->GetName());

   StringArray theSSBodies = ss->GetBodiesInUse();
   // Do this to treat SS bodies like all other objects:
   for (UnsignedInt i = 0; i < theSSBodies.size(); ++i)
      objList.push_back(theSSBodies[i]);

   
   #if DBGLVL_FINAL_PASS > 0 //------------------------------ debug ----
   MessageInterface::ShowMessage("FinalPass:: All object list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif //------------------------------------------- end debug ----
   
   //----------------------------------------------------------------------
   // Check reference objects
   //----------------------------------------------------------------------
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      obj = FindObject(*i);
      
      #if DBGLVL_FINAL_PASS > 1
      MessageInterface::ShowMessage
         ("Checking ref. object on %s:%s -> retval = \n",
               obj->GetTypeName().c_str(), obj->GetName().c_str());
      #endif
      
      // check System Parameters separately since it follows certain naming
      // convention.  "owner.dep.type" where owner can be either Spacecraft
      // or Burn for now
      if (obj == NULL)
         throw InterpreterException("The object " + (*i) + " does not exist");

      // Check attitude for singularity or disallowed values
      if (obj->IsOfType("Spacecraft"))
      {
         try
         {
            #if DBGLVL_FINAL_PASS > 1
            MessageInterface::ShowMessage
               ("Calling Validate on object %s:%s\n",
                     obj->GetTypeName().c_str(), obj->GetName().c_str());
            #endif
            obj->Validate();
         }
         catch (BaseException& ex)
         {
            HandleError(ex, false);
            retval = false;
         }
      }

      // Validate IBs so we trap mass depletion issues
      if (obj->GetType() == Gmat::IMPULSIVE_BURN)
      {
         try
         {
            obj->Validate();
         }
         catch (BaseException& ex)
         {
            HandleError(ex, false);
            retval = false;
         }
      }
            
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
               refObj = FindObject(owner);
               if (refObj == NULL)
               {
                  InterpreterException ex
                     ("Nonexistent object \"" + owner + "\" referenced in \"" +
                      obj->GetName() + "\"");
                  HandleError(ex, false);
                  retval = false;
               }
               //else if (param->GetOwnerType() != refObj->GetType())
               else if (!refObj->IsOfType(param->GetOwnerType()))
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
      
      // check Function separately since it has inputs that can be any object type,
      // including Real number (1234.5678) and String literal ('abc')
      //
      // We don't want to check this in the FinalPass(), since it will be checked
      // when ScriptInterpreter::InterpretGmatFunction() is called
      else if (obj->GetType() == Gmat::FUNCTION)
      {
         // If GmatFunction, see if function file exist and the function name
         // matches the file name
         if (obj->GetTypeName() == "GmatFunction")
         {
            std::string funcPath = obj->GetStringParameter("FunctionPath");
            #if DBGLVL_FUNCTION_DEF > 0
            MessageInterface::ShowMessage
               ("Interpreter::FinalPass() calling CheckFunctionDefinition()\n");
            #endif
            bool retval1 = CheckFunctionDefinition(funcPath, obj, false);
            retval = retval && retval1;
         }
      }
      
      //-----------------------------------------------------------------
      // Note: This section needs be modified as needed. 
      // GetRefObjectTypeArray() should be implemented if we want to
      // add to this list. This was added to write specific error messages.
      //-----------------------------------------------------------------
      else if ((obj->HasRefObjectTypeArray()))
      {
         // Set return flag to false if any check failed
         try
         {
            bool retval1 = CheckUndefinedReference(obj, false);
            retval = retval && retval1;
            
            // Subscribers uses ElementWrapper to handle Parameter, Variable,
            // Array, Array elements, so create wrappers in ValidateSubscriber()
            if (retval && obj->IsOfType(Gmat::SUBSCRIBER))
            {
               retval = retval && ValidateSubscriber(obj);
               // Since OrbitView has Validate() method
               if (!obj->Validate())
               {
                  retval = retval && false;
                  InterpreterException ex
                     (obj->GetLastErrorMessage() + " in \"" + obj->GetName() + "\"");
                  HandleError(ex, false);
               }
            }
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
               refObj = FindObject(refNameList[j]);
               if ((refObj == NULL) || !(refObj->IsOfType(Gmat::SPACE_POINT)))
               {
                  #if DBGLVL_FINAL_PASS > 1
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
            // Use exception to remove Visual C++ warning
            e.GetMessageType();
            #if DBGLVL_FINAL_PASS
            MessageInterface::ShowMessage(e.GetFullMessage());
            #endif
         }
      }

      #if DBGLVL_FINAL_PASS > 1
      MessageInterface::ShowMessage
         ("%s\n", retval ? "True" : "False");
      #endif

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
   
   #if DBGLVL_FINAL_PASS > 1
   MessageInterface::ShowMessage("FinalPass:: CalculatedPoint list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif
   
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      obj = FindObject(*i);
      refNameList = obj->GetRefObjectNameArray(Gmat::SPACE_POINT);
      
      if (obj->GetTypeName() == "LibrationPoint")
      {
         std::string primary = obj->GetStringParameter("Primary");
         std::string secondary = obj->GetStringParameter("Secondary");
         
         #if DBGLVL_FINAL_PASS > 1
         MessageInterface::ShowMessage
            ("   primary=%s, secondary=%s\n", primary.c_str(), secondary.c_str());
         #endif
         
         if (primary == secondary)
         {
            InterpreterException ex
               ("The Primary and Secondary bodies cannot be the same in the "
                "LibrationPoint \"" + obj->GetName() + "\"");
            HandleError(ex, false);
            retval = false;
         }
      }
      
      //----------------------------------------------------------------
      // Now set ref objects to CalculatedPoint objects
      //----------------------------------------------------------------
      
      #if DBGLVL_FINAL_PASS > 1
      MessageInterface::ShowMessage
         ("   Setting RefObject on obj=%s\n", obj->GetName().c_str());
      #endif
      for (UnsignedInt j = 0; j < refNameList.size(); j++)
      {
         #if DBGLVL_FINAL_PASS > 1
         MessageInterface::ShowMessage
            ("   refNameList[%d]=%s\n", j, refNameList[j].c_str());
         #endif
         
         refObj = FindObject(refNameList[j]);
         if (refObj)
            obj->SetRefObject(refObj, Gmat::SPACE_POINT, refObj->GetName());
      }
   }
   
   // Update the owned ODE models based on the fully scripted original
   objList = theModerator->GetListOfObjects(Gmat::PROP_SETUP);
   #if DBGLVL_FINAL_PASS > 1
   MessageInterface::ShowMessage("FinalPass:: PropSetup list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      obj = FindObject(*i);
      if (obj != NULL)
      {
         if (((PropSetup*)obj)->GetPropagator()->UsesODEModel())
         {
            std::string refName = obj->GetStringParameter("FM");
            GmatBase *configuredOde = FindObject(refName);
//            ODEModel *ode = ((PropSetup*)obj)->GetODEModel();
            
            #if DBGLVL_FINAL_PASS > 1
            MessageInterface::ShowMessage
               ("   OdeModel='%s', configuredOde=<%p>\n", refName.c_str(), configuredOde);
            #endif
            
            if (configuredOde != NULL)
            {
               if (configuredOde->IsOfType(Gmat::ODE_MODEL))
                  ((PropSetup*)obj)->SetODEModel(((ODEModel*)configuredOde));
               else
                  throw InterpreterException("The object named \"" +
                        refName + "\", referenced by the Propagator \"" +
                        obj->GetName() + "\" as an ODE model is the wrong "
                              "type; it is a " + configuredOde->GetTypeName());
            }
            else
            {
               if ((refName != "InternalODEModel") &&
                   (refName != "InternalForceModel"))
                  throw InterpreterException("The ODEModel named \"" +
                        refName + "\", referenced by the Propagator \"" +
                        obj->GetName() + "\" cannot be found");

               // Create default ODE model
               configuredOde = CreateObject("ODEModel", refName, 1);
               obj->SetRefObject(configuredOde, configuredOde->GetType(),
                     configuredOde->GetName());
            }
         }
      }
   }
   
   //----------------------------------------------------------------------
   // Initialize CoordinateSystem
   //----------------------------------------------------------------------
   objList = theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   
   #if DBGLVL_FINAL_PASS > 1//------------------------------ debug ----
   MessageInterface::ShowMessage("FinalPass:: CoordinateSystem list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("    %s\n", (objList.at(ii)).c_str());
   #endif //------------------------------------------- end debug ----
   
   objList = theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      CoordinateSystem *cs = (CoordinateSystem*)FindObject(*i);
      #if DBGLVL_FINAL_PASS > 1
      MessageInterface::ShowMessage("Initializing CoordinateSystem '%s'\n",
                                    i->c_str());
      #endif
      bool setCSObj = false;
      ObjectTypeArray csTypes = cs->GetRefObjectTypeArray();
      #if DBGLVL_FINAL_PASS > 1
         MessageInterface::ShowMessage("In Interpreter, cs = %s\n", cs->GetName().c_str());
         for (unsigned int jj = 0; jj < csTypes.size(); jj++)
            MessageInterface::ShowMessage("   ref type %d = %d\n", (Integer) jj,
                                         (Integer) csTypes[jj]);
      #endif
      for (UnsignedInt ii = 0; ii < csTypes.size(); ii++)
      {
         #if DBGLVL_FINAL_PASS > 1
            MessageInterface::ShowMessage(
                  "In Interpreter, getting ref object names for type %d\n",
                  (Integer) csTypes.at(ii));
         #endif
         refNameList.clear();
         refNameList = cs->GetRefObjectNameArray(csTypes.at(ii));
         for (UnsignedInt j = 0; j < refNameList.size(); j++)
         {
            #if DBGLVL_FINAL_PASS > 1
            MessageInterface::ShowMessage
               ("   refNameList[%d]=%s\n", j, refNameList[j].c_str());
            #endif

            refObj = FindObject(refNameList[j]);
            if ((refObj == NULL) || !(refObj->IsOfType(csTypes.at(ii))))
            {
               InterpreterException ex;
               // Checking for undefined ref objects already done for CoordinateSystem
               // so commented out to avoid duplicated message (LOJ: 2009.12.17)
               // UNCOMMENTED for GMT-3462 Error message just says error occurred and
               // no other detail provided (TGG: 2013-01-25)
               if (csTypes.at(ii) == Gmat::SPACE_POINT)
               {
                  ex.SetDetails
                     ("Nonexistent SpacePoint \"" + refNameList[j] +
                      "\" referenced in the CoordinateSystem \"" + cs->GetName() + "\"");
               }
               else
               {
                  ex.SetDetails
                     ("Nonexistent Reference Object \"" + refNameList[j] +
                      "\" referenced in the CoordinateSystem \"" + cs->GetName() + "\"");
               }
               HandleError(ex, false);
               retval = false;
            }
            else
            {
               #if DBGLVL_FINAL_PASS > 1
               MessageInterface::ShowMessage
                  ("   setting %s on CS %s\n", refNameList[j].c_str(), cs->GetName().c_str());
               #endif
               cs->SetRefObject(refObj, csTypes.at(ii), refObj->GetName());
//               cs->Initialize();
               setCSObj = true;
            }
         }
      }
      if (setCSObj)
         cs->Initialize();

//      refNameList = cs->GetRefObjectNameArray(Gmat::SPACE_POINT);
//      for (UnsignedInt j = 0; j < refNameList.size(); j++)
//      {
//         #if DBGLVL_FINAL_PASS > 1
//         MessageInterface::ShowMessage
//            ("   refNameList[%d]=%s\n", j, refNameList[j].c_str());
//         #endif
//
//         refObj = FindObject(refNameList[j]);
//         if ((refObj == NULL) || !(refObj->IsOfType(Gmat::SPACE_POINT)))
//         {
//            // Checking for undefined ref objects already done for CoordinateSystem
//            // so commented out to avoid duplicated message (LOJ: 2009.12.17)
//            // UNCOMMENTED for GMT-3462 Error message just says error occurred and
//			// no other detail provided (TGG: 2013-01-25)
//            InterpreterException ex
//               ("Nonexistent SpacePoint \"" + refNameList[j] +
//                "\" referenced in the CoordinateSystem \"" + cs->GetName() + "\"");
//            HandleError(ex, false);
//            retval = false;
//         }
//         else
//         {
//            #if DBGLVL_FINAL_PASS > 1
//            MessageInterface::ShowMessage
//               ("   setting %s on CS %s\n", refNameList[j].c_str(), cs->GetName().c_str());
//            #endif
//            cs->SetRefObject(refObj, Gmat::SPACE_POINT, refObj->GetName());
//            cs->Initialize();
//         }
//      }
   }

   //-------------------------------------------------------------------
   // Special case for BodyFixedPoints, we need to set CoordinateSystem
   // pointers for the BodyFixed and the MJ2000Eq coordinate systems.  In
   // addition, we need to pass the pointer to the central body.
   //-------------------------------------------------------------------
   #if DBGLVL_FINAL_PASS > 1
      MessageInterface::ShowMessage("FinalPass:: about to get BodyFixedPoint list\n");
   #endif
   objList = theModerator->GetListOfObjects(Gmat::BODY_FIXED_POINT);

   #if DBGLVL_FINAL_PASS > 1
   MessageInterface::ShowMessage("FinalPass:: BodyFixedPoint list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
   MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif

   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      obj = FindObject(*i);

      StringArray csNames = obj->GetRefObjectNameArray(Gmat::COORDINATE_SYSTEM);
      for (StringArray::iterator csName = csNames.begin();
           csName != csNames.end(); ++csName)
      {
         GmatBase *csObj = FindObject(*csName);

         // To catch as many errors we can, continue with next object
         if (csObj == NULL)
            continue;

         #if DBGLVL_FINAL_PASS > 1
               MessageInterface::ShowMessage
               ("   Calling '%s'->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
                csObj->GetName().c_str(), csObj, csObj->GetType());
         #endif

         if (csObj->GetType() != Gmat::COORDINATE_SYSTEM)
         {
            InterpreterException ex
            ("The BodyFixedPoint \"" + obj->GetName() + "\" failed to set "
             "\"CoordinateSystem\" to \"" + *csName + "\"");
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
            ("The BodyFixedPoint \"" + obj->GetName() + "\" failed to set "
             "CoordinateSystem: " + e.GetFullMessage());
            HandleError(ex, false);
            retval = false;
            continue;
         }
      }
      std::string cbName = obj->GetRefObjectName(Gmat::CELESTIAL_BODY);
      GmatBase *cbObj = FindObject(cbName);

      #if DBGLVL_FINAL_PASS > 1
         MessageInterface::ShowMessage
         ("   Calling '%s'->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
          cbObj->GetName().c_str(), cbObj, cbObj->GetType());
      #endif

      if ((cbObj == NULL) || (cbObj->GetType() != Gmat::CELESTIAL_BODY))
      {
         InterpreterException ex
         ("The BodyFixedPoint \"" + obj->GetName() + "\" failed to set "
          "\"CelestialBody\" to \"" + cbName + "\"");
         HandleError(ex, false);
         retval = false;
         continue;
      }

      try
      {
         obj->SetRefObject(cbObj, Gmat::CELESTIAL_BODY, cbObj->GetName());
      }
      catch (BaseException &e)
      {
         InterpreterException ex
         ("The BodyFixedPoint \"" + obj->GetName() + "\" failed to set "
          "CelestialBody: " + e.GetFullMessage());
         HandleError(ex, false);
         retval = false;
         continue;
      }
   }

   //-------------------------------------------------------------------
   // Special case for Spacecraft, we need to set CoordinateSystem
   // pointer in which initial state is represented.  So that
   // Spacecraft can convert initial state in user representation to
   // internal representation (EarthMJ2000Eq Cartesian).
   //-------------------------------------------------------------------
   objList = theModerator->GetListOfObjects(Gmat::SPACECRAFT);

   #if DBGLVL_FINAL_PASS > 1
   MessageInterface::ShowMessage("FinalPass:: Spacecraft list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif


   // Ordering matters for the Spacecraft state: Celestial body based CS should
   // used before others, so that states that depend of spacecraft origins get
   // set after those spacecraft have a change to be set up.  The following code
   // does a pre-sort for this, and might need further refinements over time.
   ObjectArray scObjects;
   Integer cbOrigined = 0;
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      obj = FindObject(*i);
      if (obj != NULL)
      {
         GmatBase* csObj = FindObject(obj->GetStringParameter("CoordinateSystem"));
         if (csObj && (!csObj->IsOfType("CoordinateSystem")))
         {
            InterpreterException ex
               ("The Spacecraft \"" + obj->GetName() + "\" failed to set "
                "\"CoordinateSystem\" to \"" + csObj->GetName() + "\"");
            HandleError(ex, false);
            retval = false;
            continue;
         }
         CoordinateSystem *cs = (CoordinateSystem*) csObj;
         if (cs != NULL)
         {
            #if DBGLVL_FINAL_PASS > 1
            MessageInterface::ShowMessage("FinalPass:: cs is %s\n", cs->GetName().c_str());
            #endif
            if (cs->HasCelestialBodyOrigin())
            {
               scObjects.insert(scObjects.begin(), obj);
               ++cbOrigined;
            }
            else
               scObjects.push_back(obj);
         }
         else
         {
            #if DBGLVL_FINAL_PASS > 1
            MessageInterface::ShowMessage("FinalPass:: cs \"%s\" is NULL\n",
                  (obj->GetStringParameter("CoordinateSystem")).c_str());
            #endif
            scObjects.push_back(obj);
         }
      }
   }

   for (UnsignedInt i = 0; i < scObjects.size(); ++i)
   {
      obj = scObjects[i];

      // Now we have more than one CoordinateSystem from Spacecraft.
      // In additions to Spacecraft's CS, it has to handle CS from Thrusters
      // and Attitude. (LOJ: 2009.09.24)
      StringArray csNames = obj->GetRefObjectNameArray(Gmat::COORDINATE_SYSTEM);
      for (StringArray::iterator csName = csNames.begin();
           csName != csNames.end(); ++csName)
      {
         GmatBase *csObj = FindObject(*csName);

         // To catch as many errors we can, continue with next object
         if (csObj == NULL)
            continue;

         #if DBGLVL_FINAL_PASS > 1
         MessageInterface::ShowMessage
            ("   Calling '%s'->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
             csObj->GetName().c_str(), csObj, csObj->GetType());
         #endif

         if (csObj->GetType() != Gmat::COORDINATE_SYSTEM)
         {
            InterpreterException ex
               ("The Spacecraft \"" + obj->GetName() + "\" failed to set "
                "\"CoordinateSystem\" to \"" + *csName + "\"");
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
   }

   //-------------------------------------------------------------------
   // Special case for Thrusters, we need to set CoordinateSyatem
   // pointer or Origin in which burn direction is represented.  So that
   // Thruster can convert the thrust direction in user representation to
   // internal representation (MJ2000Eq Cartesian).
   //-------------------------------------------------------------------
   objList = theModerator->GetListOfObjects(Gmat::THRUSTER);

   #if DBGLVL_FINAL_PASS > 1
   MessageInterface::ShowMessage("FinalPass:: Thruster list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif

   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      obj = FindObject(*i);

      StringArray csNames = obj->GetRefObjectNameArray(Gmat::COORDINATE_SYSTEM);
      for (StringArray::iterator csName = csNames.begin();
           csName != csNames.end(); ++csName)
      {
         GmatBase *csObj = FindObject(*csName);

         // To catch as many errors we can, continue with next object
         if (csObj == NULL)
         {
            InterpreterException ex
               ("The CoordinateSystem \"" + *csName + "\" for the Thruster \"" +
                obj->GetName() + "\" could not be found");
            HandleError(ex, false);
            retval = false;
            continue;
         }

         #if DBGLVL_FINAL_PASS > 1
         MessageInterface::ShowMessage
            ("   Calling '%s'->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
             csObj->GetName().c_str(), csObj, csObj->GetType());
         #endif

         if (csObj->GetType() != Gmat::COORDINATE_SYSTEM)
         {
            InterpreterException ex
               ("The Thruster \"" + obj->GetName() + "\" failed to set "
                "\"CoordinateSystem\" to \"" + *csName + "\"");
            HandleError(ex, false);
            retval = false;
            continue;
         }
      }

      StringArray cbNames = obj->GetRefObjectNameArray(Gmat::CELESTIAL_BODY);
      for (StringArray::iterator cbName = cbNames.begin();
            cbName != cbNames.end(); ++cbName)
      {
         GmatBase *cbObj = FindObject(*cbName);

         // To catch as many errors we can, continue with next object
         if (cbObj == NULL)
         {
            InterpreterException ex
               ("The Origin \"" + *cbName + "\" for the Thruster \"" +
                obj->GetName() + "\" could not be found");
            HandleError(ex, false);
            retval = false;
            continue;
         }

         #if DBGLVL_FINAL_PASS > 1
         MessageInterface::ShowMessage
            ("   Calling '%s'->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
             cbObj->GetName().c_str(), cbObj, cbObj->GetType());
         #endif

         if (cbObj->GetType() != Gmat::CELESTIAL_BODY)
         {
            InterpreterException ex
               ("The Thruster \"" + obj->GetName() + "\" failed to set "
                "\"Origin\" to \"" + *cbName + "\"");
            HandleError(ex, false);
            retval = false;
            continue;
         }
      }

      StringArray tankNames = obj->GetStringArrayParameter("Tank");
      for (StringArray::iterator theName = tankNames.begin();
            theName != tankNames.end(); ++theName)
      {
         GmatBase *theObj = FindObject(*theName);

         // To catch as many errors we can, continue with next object
         if (theObj == NULL)
         {
            InterpreterException ex
               ("The Tank \"" + *theName + "\" for the Thruster \"" +
                obj->GetName() + "\" could not be found");
            HandleError(ex, false);
            retval = false;
            continue;
         }

         #if DBGLVL_FINAL_PASS > 1
         MessageInterface::ShowMessage
            ("   Calling '%s'->SetRefObject(%s(%p), %d)\n", obj->GetName().c_str(),
             theObj->GetName().c_str(), theObj, theObj->GetType());
         #endif

         if (theObj->GetType() != Gmat::FUEL_TANK)
         {
            InterpreterException ex
               ("The Thruster \"" + obj->GetName() + "\" failed to set "
                "\"Tank\" to \"" + *theName + "\"");
            HandleError(ex, false);
            retval = false;
            continue;
         }
      }
   }

   //-------------------------------------------------------------------
   // Validate membership uniqueness for Formations.
   //-------------------------------------------------------------------
   objList = theModerator->GetListOfObjects(Gmat::FORMATION);
   if (objList.size() > 0)
   {
      StringArray f1List, f2List;
      GmatBase *f1, *f2;
      std::string overlaps;

      // Make sure formation members are all SpaceObjects (note: Formations can
      // no longer contain formations, at least for R2013a)
      for (UnsignedInt i = 0; i < objList.size(); ++i)
      {
         f1 = theModerator->GetConfiguredObject(objList[i]);
         f1List = f1->GetStringArrayParameter("Add");

         for (UnsignedInt j = 0; j < f1List.size(); ++j)
         {
            obj = theModerator->GetConfiguredObject(f1List[j]);
            if (obj != NULL)
            {
//               if (obj->IsOfType("SpaceObject") == false)
               if (obj->IsOfType("Spacecraft") == false)
               {
                  if (obj->IsOfType("Formation"))
                     overlaps += "   GMAT does not allow Formations of "
                            "Formations, so the Formation \"" + obj->GetName() +
                            "\" cannot be added to the Formation \"" +
                            f1->GetName() + "\".";
                  else
                     overlaps += "   " + f1List[j] + " cannot propagate in the "
                           "formation " + f1->GetName() + ".\n";
               }
            }
         }
      }

      // Make sure the are no shared objects between Formations
      for (UnsignedInt i = 0; i < objList.size()-1; ++i)
      {
         f1 = theModerator->GetConfiguredObject(objList[i]);
         f1List = f1->GetStringArrayParameter("Add");
         for (UnsignedInt j = i+1; j < objList.size(); ++j)
         {
            f2 = theModerator->GetConfiguredObject(objList[j]);
            f2List = f2->GetStringArrayParameter("Add");

            // Check member by member for overlap
            for (UnsignedInt m = 0; m < f1List.size(); ++m)
            {
               if (find(f2List.begin(), f2List.end(), f1List[m])!=f2List.end())
               {
                  overlaps += "   ";
                  overlaps += f1List[m] + " is in formations " + f1->GetName() +
                              " and " + f2->GetName() + "\n";
               }
            }
         }
      }
      if (overlaps.length() > 0)
      {
         InterpreterException ex("Formation errors:\n" + overlaps);
         HandleError(ex, false);
         retval = false;
      }
   }
   
   //-------------------------------------------------------------------
   // For EphemerisFile validation, we need to do this after CoordinateSystems
   // are initialized, so that EphemerisFile can query for the origin name
   // for validation.
   //-------------------------------------------------------------------
   retval = retval && FinalPassSubscribers();
   
   
   //-------------------------------------------------------------------
   // Special case for SolverBranchCommand such as Optimize, Target,
   // we need to set Solver object to SolverBranchCommand and then
   // to all Vary commands inside. Since Vary command's parameters are
   // different depends on the type of the Solver, such as
   // DifferentialCorrector or Optimizer. When user saves the script
   // without running, it will not write correctly since the Solve is
   // not set, so set it here.
   //-------------------------------------------------------------------
   GmatCommand *current = theModerator->GetFirstCommand();
   while (current != NULL)
   {
      if ((current->GetChildCommand(0)) != NULL)
         SetObjectInBranchCommand(current, "SolverBranchCommand", "Vary",
                                  "SolverName");
      
      current = current->GetNext();
   }
   
   // Validate the references used in the commands
   try
   {
      if (ValidateMcsCommands(theModerator->GetFirstCommand()) == false)
         retval = false;

      current = theModerator->GetFirstCommand();
      while (current != NULL)
      {
         if (current->IsOfType("BranchCommand"))
            if (!ValidateSolverCmdLevels(current, 0))
               retval = false;
         current = current->GetNext();
      }

   }
   catch (BaseException &ex)
   {
      HandleError(ex, false, false);
      retval = false;
   }


   #if DBGLVL_FINAL_PASS
   MessageInterface::ShowMessage("Interpreter::FinalPass() returning %s\n",
         retval ? "true" : "false");
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// bool FinalPassSubscribers()
//------------------------------------------------------------------------------
/**
 * Checks for non-existent objects or disallowed value for Subscribers in parsing mode.
 *
 * @throws exception when validation fails
 */
//------------------------------------------------------------------------------
bool Interpreter::FinalPassSubscribers()
{
   bool retval = true;
   StringArray objList = theModerator->GetListOfObjects(Gmat::EPHEMERIS_FILE);
   
   #if DBGLVL_FINAL_PASS > 1
   MessageInterface::ShowMessage("FinalPassSubscribers:: EphemerisFile list =\n");
   for (Integer ii = 0; ii < (Integer) objList.size(); ii++)
      MessageInterface::ShowMessage("   %s\n", (objList.at(ii)).c_str());
   #endif
   
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      GmatBase *obj = FindObject(*i);
      if (obj != NULL)
      {
         try
         {
            StringArray objNames = obj->GetRefObjectNameArray(Gmat::SPACECRAFT);
            for (unsigned int i = 0; i < objNames.size(); i++)
            {
               GmatBase *satObj = FindObject(objNames[i]);
               if (satObj)
                  obj->SetRefObject(satObj, Gmat::SPACECRAFT, satObj->GetName());
               else
               {
                  InterpreterException ex;
                  if (objNames[i] == "")
                  {
                     ex.SetDetails("The Spacecraft was not set in EphemerisFile \"%s\"",
                                   obj->GetName().c_str());
                  }
                  else
                  {
                     ex.SetDetails("Nonexistent Spacecraft object \"%s\" referenced in \"%s\"",
                                   objNames[i].c_str(), obj->GetName().c_str());
                  }
                  HandleError(ex, false);
                  retval = false;
               }
            }
            
            objNames = obj->GetRefObjectNameArray(Gmat::COORDINATE_SYSTEM);
            for (unsigned int i = 0; i < objNames.size(); i++)
            {
               GmatBase *csObj = FindObject(objNames[i]);
               if (csObj)
                  obj->SetRefObject(csObj, Gmat::COORDINATE_SYSTEM, csObj->GetName());
               else
               {
                  InterpreterException ex
                     ("Nonexistent CoordinateSystem object \"" + objNames[i] + "\" referenced in \"" +
                      obj->GetName() + "\"");
                  HandleError(ex, false);
                  retval = false;
               }
            }
            
            obj->Validate();
         }
         catch (BaseException& ex)
         {
            HandleError(ex, false);
            retval = false;
         }
      }
   }
   
   return retval;
}

//------------------------------------------------------------------------------
// bool Interpreter::ValidateMcsCommands(GmatCommand *first)
//------------------------------------------------------------------------------
/**
 * Checks that the commands in the Mission Control Sequence were built
 * acceptably when parsed.
 *
 * Note that acceptability at this level is necessary but not sufficient for a
 * control sequence to run.  Some commands need additional information,
 * generated in the Sandbox or during the run, to proceed.
 *
 * @param first The command at the start of the control sequence
 *
 * @return true if the command references have been validated and the commands
 *         were set up acceptably, false if not.
 */
//------------------------------------------------------------------------------
bool Interpreter::ValidateMcsCommands(GmatCommand *first, GmatCommand *parent,
      StringArray *missingObjects, std::string *accumulatedErrors)
{
   bool retval = true, cleanMissingObj = false, cleanAccError = false;
   GmatCommand *current = first;

   StringArray theObjects =
         theModerator->GetListOfObjects(Gmat::UNKNOWN_OBJECT);

   SolarSystem *ss = theModerator->GetSolarSystemInUse();
   theObjects.push_back(ss->GetName());

   StringArray theSSBodies = ss->GetBodiesInUse();
   // Do this to treat SS bodies like all other objects:
   for (UnsignedInt i = 0; i < theSSBodies.size(); ++i)
      theObjects.push_back(theSSBodies[i]);
   #ifdef DEBUG_ALL_OBJECTS
      for (unsigned int ii = 0; ii < theObjects.size(); ii++)
         MessageInterface::ShowMessage(" Obj %d :  %s\n", (Integer) ii,
               (theObjects.at(ii)).c_str());
   #endif

   Integer beginMCSCount = 0;

   if (missingObjects == NULL)
   {
      missingObjects = new StringArray;
      cleanMissingObj = true;
   }
   if (accumulatedErrors == NULL)
   {
      accumulatedErrors = new std::string;
      cleanAccError = true;
   }

   Integer errorCount, validationErrorCount = 0;

   do
   {
      if (theModerator->IsSequenceStarter(current->GetTypeName()))
         ++beginMCSCount;

      StringArray refs;
      // Validate that objects exist for object references
      if (current)
      {
         #ifdef DEBUG_COMMAND_VALIDATION
            MessageInterface::ShowMessage("Checking \"%s\"; refs:\n", current->
                  GetGeneratingString(Gmat::NO_COMMENTS, "", "").c_str());
         #endif

         errorCount = 0;
         refs = current->GetObjectList();
         #ifdef DEBUG_COMMAND_VALIDATION
            for (UnsignedInt i = 0; i < refs.size(); ++i)
               MessageInterface::ShowMessage("   %s\n", refs[i].c_str());
         #endif

         std::string missing;

         for (UnsignedInt i = 0; i < refs.size(); ++i)
         {
            #ifdef DEBUG_COMMAND_VALIDATION
               MessageInterface::ShowMessage("   Looking for %s...",
                     refs[i].c_str());
            #endif
            // Check to see if each referenced object exists
            if (find(theObjects.begin(), theObjects.end(),
                  refs[i]) == theObjects.end())
            {
               if (missing.length() == 0)
               {
                  missing = "      \"";
                  missing += current->GetGeneratingString(Gmat::NO_COMMENTS);
                  missing += "\" references missing object(s):";
                  ++errorCount;
                  retval = false;
               }
               if (errorCount == 1)
                  missing += "  " + refs[i];
               else
                  missing += ", " + refs[i];
               #ifdef DEBUG_COMMAND_VALIDATION
                  MessageInterface::ShowMessage("missing\n");
               #endif
            }
            else
            {
               GmatBase* obj = theModerator->GetConfiguredObject(refs[i]);
               if (current->AcceptsObjectType(obj->GetType()) == false)
                  MessageInterface::ShowMessage("Configured object type not allowed\n");
               #ifdef DEBUG_COMMAND_VALIDATION
                  else
                     MessageInterface::ShowMessage("Found!\n");
               #endif
            }
         }

         if (missing.length() > 0)
         {
            std::stringstream msg;
            msg << errorCount << ": " << missing;
            missingObjects->push_back(missing);
         }
      }

      try
      {
         if (current->IsOfType("BranchCommand"))
         {
            retval &= ValidateMcsCommands(current->GetChildCommand(0), current,
                  missingObjects, accumulatedErrors);
         }
      }
      catch (BaseException &)
      {
         // Ignore the derived exception
      }

      // Call the command's Validate method to check internal validity
      if (current->Validate() == false)
      {
         std::string errmsg = current->GetLastErrorMessage();
         current->SetLastErrorMessage("");
         #ifdef DEBUG_COMMAND_VALIDATION
            MessageInterface::ShowMessage("The command \"%s\" failed validation\n",
                  current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif
         (*accumulatedErrors) += "   The command \"" +
               current->GetGeneratingString(Gmat::NO_COMMENTS) +
               "\" failed validation. " + errmsg + "\n";
         ++validationErrorCount;
         retval = false;
      }

      current = current->GetNext();
   }
   while ((current != NULL) && (current != first) && (current != parent));

   std::string exceptionError = (*accumulatedErrors);

   if ((missingObjects->size() > 0) || (validationErrorCount > 0) ||
       (beginMCSCount > 1))  // for now -- use this when BMS is REQUIRED:
//       (beginMCSCount != 1))
   {
      #ifdef DEBUG_VALIDATION
         MessageInterface::ShowMessage("\n   Missing objects:\n");
      #endif
      if (missingObjects->size() > 0)
      {
         exceptionError += "   Possible missing objects referenced:\n";
         for (UnsignedInt i = 0; i < missingObjects->size(); ++i)
         {
            #ifdef DEBUG_VALIDATION
               MessageInterface::ShowMessage("      %s\n",
                     missingObjects[i].c_str());
            #endif
            exceptionError += (*missingObjects)[i] + "\n";
         }
      }

      if (cleanMissingObj)
         delete missingObjects;
      if (cleanAccError)
         delete accumulatedErrors;

      if (beginMCSCount > 1)
         exceptionError += "Too many Mission Sequence start "
               "commands (from the list [" + 
               theModerator->GetStarterStringList() + "]) were found";

      if (beginMCSCount == 0)
         exceptionError += "No Mission Sequence starter commands (from the "
         "list [" + theModerator->GetStarterStringList() + "]) were found";

      throw InterpreterException("\n" + exceptionError);
   }

   if (cleanMissingObj)
      delete missingObjects;
   if (cleanAccError)
      delete accumulatedErrors;

   return retval;
}


//------------------------------------------------------------------------------
// bool Interpreter::ValidateSolverCmdLevels(GmatCommand *sbc, Integer cmdLevel)
//------------------------------------------------------------------------------
/**
 * Validates that Solver commands are at the Solver Control Sequence level
 *
 * @param sbc A BranchCommand at the head of the search
 * @param cmdLevel Depth into the branch being examined
 *
 * @return true if the levels are correct, false if not
 */
//------------------------------------------------------------------------------
bool Interpreter::ValidateSolverCmdLevels(GmatCommand *sbc, Integer cmdLevel)
{
   bool retval = true;

   std::string solverName = "";

   // Handle the solver branch
   if (sbc->IsOfType("SolverBranchCommand"))
   {
      solverName = sbc->GetStringParameter("SolverName");

      // March through the SCS
      GmatCommand *current = sbc->GetChildCommand(0);
      while ((current != NULL) && (current != sbc))
      {
         if (current->IsOfType("BranchCommand"))
            retval = ValidateSolverCmdLevels(current, cmdLevel+1);
         else
         {
            // Check that solver names match
            if (current->IsOfType("SolverSequenceCommand"))
            {
               if (current->GetStringParameter("SolverName") != solverName)
               {
                  std::string generator = current->GetGeneratingString();
                  UnsignedInt loc = generator.find("{SOLVER IS",0);
                  if (loc != std::string::npos)
                  {
                     generator = generator.substr(0,loc);
                     generator = generator + "...";
                  }

                  throw InterpreterException("The Solver \"" +
                        current->GetStringParameter("SolverName") +
                        "\" in the " + current->GetTypeName() + " command does "
                        "not match the Solver \"" + solverName +
                        "\" that starts its Solver Control Sequence on "
                        "the line\n" + generator);
               }
            }
         }
         current = current->GetNext();
      }

      solverName = "";
   }
   // All other branch commands increment the level and perform checks
   else
   {
      GmatCommand *current = sbc->GetChildCommand(0);
      while ((current != NULL) && (current != sbc))
      {
         if (current->IsOfType("BranchCommand"))
            retval = ValidateSolverCmdLevels(current, cmdLevel+1);
         else
         {
            if (current->IsOfType("SolverSequenceCommand"))
            {
               std::string generator = current->GetGeneratingString();

               UnsignedInt loc = generator.find("{SOLVER IS",0);
               if (loc != std::string::npos)
               {
                  generator = generator.substr(0,loc);
                  generator = generator + "...";
               }
               throw InterpreterException("The command \"" +
                     generator + "\" is a "
                     "Solver command, and cannot be nested inside a \"" +
                     sbc->GetTypeName() + "\" Control Logic command");
            }
         }
         current = current->GetNext();
      }
   }
   return retval;
}

//------------------------------------------------------------------------------
// void SetObjectInBranchCommand(GmatCommand *brCmd,
//       const std::string &branchType, const std::string childType,
//       const std::string &objName)
//------------------------------------------------------------------------------
void Interpreter::SetObjectInBranchCommand(GmatCommand *brCmd,
                                           const std::string &branchType,
                                           const std::string &childType,
                                           const std::string &objName)
{
   #ifdef DEBUG_BRANCH_COMMAND_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectInBranchCommand() entered, brCmd=<%p><%s>, branchType='%s', "
       "childType='%s'\n", brCmd, brCmd->GetTypeName().c_str(), branchType.c_str(),
       childType.c_str());
   #endif
   
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   GmatBase *solver = NULL;
   std::string solverName;
   
   if (brCmd->IsOfType(branchType))
   {
      #ifdef DEBUG_BRANCH_COMMAND_OBJECT
      MessageInterface::ShowMessage
         ("==> found type of '%s'\n", brCmd, brCmd->GetTypeName().c_str());
      #endif
      
      solverName = brCmd->GetStringParameter(objName);
      solver = FindObject(solverName);
      
      #ifdef DEBUG_BRANCH_COMMAND_OBJECT
      MessageInterface::ShowMessage
         ("   Found solver <%p><%s>'%s'\n", solver, solver ? solver->GetTypeName().c_str() :
          "NULL", solver ? solver->GetName().c_str() : "NULL");
      #endif
   }
   
   while((child = current->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         #ifdef DEBUG_BRANCH_COMMAND_OBJECT
         MessageInterface::ShowMessage
            ("   nextInBranch=<%p><%s>\n", nextInBranch, nextInBranch->GetTypeName().c_str());
         #endif
         
         if (nextInBranch->GetTypeName() == childType)
         {
            #ifdef DEBUG_BRANCH_COMMAND_OBJECT
            MessageInterface::ShowMessage
               ("   found '%s', setting <%p>'%s' to <%p><%s>\n", childType.c_str(),
                solver, solver ? solver->GetName().c_str() : "NULL", nextInBranch,
                nextInBranch->GetTypeName().c_str());
            #endif
            
            if (solver != NULL)
               nextInBranch->SetRefObject(solver, Gmat::SOLVER, solver->GetName());
         }
         
         if (nextInBranch->GetChildCommand() != NULL)
            SetObjectInBranchCommand(nextInBranch, branchType, childType, objName);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
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
   
   if (type == "TrackingData")
      return true;

   if (theSolarSystem->IsBodyInUse(type))
      return true;

   if (find(allObjectTypeList.begin(), allObjectTypeList.end(), type) !=
       allObjectTypeList.end())
      return true;
   
   return false;
}


//------------------------------------------------------------------------------
// bool IsCommandType(const std::string &type)
//------------------------------------------------------------------------------
/*
 * Returns true if input string is one of Command type that can be created.
 */
//------------------------------------------------------------------------------
bool Interpreter::IsCommandType(const std::string &type)
{
   if (find(commandList.begin(), commandList.end(), type) == commandList.end())
      return false;
   
   return true;
}


//------------------------------------------------------------------------------
// Gmat::ObjectType GetObjectType(const std::string &type)
//------------------------------------------------------------------------------
/*
 * Returns corresponding Gmat::ObjectType, or Gmat::UNKNOWN_OBJECT if type is
 * not valid object type name.
 */
//------------------------------------------------------------------------------
Gmat::ObjectType Interpreter::GetObjectType(const std::string &type)
{
   if (objectTypeMap.find(type) != objectTypeMap.end())
      return objectTypeMap[type];
   else
      return Gmat::UNKNOWN_OBJECT;
}

//----------------------------------
// Private
//----------------------------------

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
   return theValidator->IsParameterType(desc);
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
         // Adding a default indicator to the string here, so that the HarmonicField object
         // can tell when it is reading a default file vs. one the user specified
         value = defaultIndicator + val;
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
// void WriteStringArray(const std::string &title1, const std::string &title2,
//                       const StringArray &parts)
//------------------------------------------------------------------------------
void Interpreter::WriteStringArray(const std::string &title1,
                                   const std::string &title2,
                                   const StringArray &parts)
{
   MessageInterface::ShowMessage("   ========== %s%s, has %d parts\n",
                                 title1.c_str(), title2.c_str(), parts.size());
   for (UnsignedInt i=0; i<parts.size(); i++)
      MessageInterface::ShowMessage("   %d: '%s'\n", i, parts[i].c_str());
   MessageInterface::ShowMessage("\n");
}


//------------------------------------------------------------------------------
// void WriteForceModel(GmatBase *obj)
//------------------------------------------------------------------------------
void Interpreter::WriteForceModel(GmatBase *obj)
{
   ODEModel *fm = (ODEModel*)obj;
   Integer numForces = fm->GetNumForces();
   MessageInterface::ShowMessage
      ("   ODEModel '%s' has %d forces\n", fm->GetName().c_str(), numForces);
   for (int i = 0; i < numForces; i++)
   {
      const PhysicalModel* force = fm->GetForce(i);
      MessageInterface::ShowMessage
         ("      force[%d] = <%p><%s>'%s'\n", i, force, force->GetTypeName().c_str(),
          force->GetName().c_str());
   }
}


//------------------------------------------------------------------------------
// bool CheckFunctionDefinition(const std::string &funcPath, GmatBase *function,
//                              bool fullCheck)
//------------------------------------------------------------------------------
/*
 * Opens function file and checks if it has valid function definition line.
 *
 * @param  funcPath  The full path and name of function file
 * @param  function  The Function pointer
 * @param  fullCheck set to true if checking fully for input and output arguments
 *
 */
//------------------------------------------------------------------------------
bool Interpreter::CheckFunctionDefinition(const std::string &funcPath,
                                          GmatBase *function, bool fullCheck)
{
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage
      ("Interpreter::CheckFunctionDefinition() function=<%p>, fullCheck=%d\n   "
       "funcPath=<%s>\n", function, fullCheck, funcPath.c_str());
   #endif
   
   debugMsg = "In CheckFunctionDefinition()";
   bool retval = true;
   
   if (function == NULL)
   {
      MessageInterface::ShowMessage
         ("** INTERNAL ERROR ** Cannot check function definition. "
          "function pointer is NULL\n");
      retval = false;;
   }
   
   // check if function path exist
   if (!GmatFileUtil::DoesFileExist(funcPath))
   {
      InterpreterException ex
         ("Nonexistent GmatFunction file \"" + funcPath +
          "\" referenced in \"" + function->GetName() + "\"\n");
      HandleError(ex, false);
      retval = false;
   }
   
   // check for no extension of .gmf or wrong extension
   StringArray parts = GmatStringUtil::SeparateBy(funcPath, ".");
   if ((parts.size() == 1) ||
       (parts.size() == 2 && parts[1] != "gmf"))
   {
      InterpreterException ex
         ("The GmatFunction file \"" + funcPath + "\" has no or incorrect file "
          "extension referenced in \"" + function->GetName() + "\"\n");
      HandleError(ex, false);
      retval = false;
   }
   
   if (!retval || !fullCheck)
   {
      #if DBGLVL_FUNCTION_DEF > 0
      MessageInterface::ShowMessage
         ("Interpreter::CheckFunctionDefinition() returning %d, fullCheck=%d\n",
          retval, fullCheck);
      #endif
      return retval;
   }
   
   // check function declaration
   std::ifstream inStream(funcPath.c_str());
   std::string line;
   StringArray inputArgs;
   StringArray outputArgs;
   
   while (!inStream.eof())
   {
      // Use cross-platform getline()
      if (!GmatFileUtil::GetLine(&inStream, line))
      {
         InterpreterException ex
            ("Error reading the GmatFunction file \"" +
             funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
         HandleError(ex, false);
         retval = false;
         break;
      }
      
      #if DBGLVL_FUNCTION_DEF > 1
      MessageInterface::ShowMessage("   line=<%s>\n", line.c_str());
      #endif
      
      line = GmatStringUtil::Trim(line, GmatStringUtil::BOTH, true, true);
      
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
      WriteStringArray("GmatFunction parts", "", parts);
      #endif
         
      StringArray lhsParts;
      
      try
      {
         lhsParts = theTextParser.Decompose(parts[0], "[]", false);
      }
      catch (BaseException &)
      {
         InterpreterException ex
            ("Invalid output argument list found in the GmatFunction file \"" +
             funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
         HandleError(ex, false);
         retval = false;
         break;
      }
      
      StringArray::size_type numLeft = lhsParts.size();
      
      #if DBGLVL_FUNCTION_DEF > 1
      WriteStringArray("GmatFunction lhsParts", "", lhsParts);
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
            ("The \"function\" is missing in the GmatFunction file \"" +
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
      
      if (hasOutput)
      {
         try
         {
            outputArgs =
               theTextParser.SeparateBrackets(lhsParts[1], "[]", ",");
            
            #if DBGLVL_FUNCTION_DEF > 1
            WriteStringArray("GmatFunction outputArgs", "", outputArgs);
            #endif
         }
         catch (BaseException &)
         {
            InterpreterException ex
               ("Invalid output argument list found in the GmatFunction file \"" +
                funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
            HandleError(ex, false);
            retval = false;
            break;
         }
         
         
         if (outputArgs.size() == 0)
         {
            InterpreterException ex
               ("The output argument list is empty in the GmatFunction file \"" +
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
            ("The function name not found in the GmatFunction file \"" +
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
         WriteStringArray("GmatFunction rhsParts", "", rhsParts);
         #endif         
      }
      catch (BaseException &)
      {
         InterpreterException ex
            ("The invalid input argument list found in the GmatFunction file \"" +
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
         ("   fileFuncName = %s', funcName = '%s'\n\n", fileFuncName.c_str(), funcName.c_str());
      #endif
      
      if (fileFuncName != funcName)
      {
         InterpreterException ex
            ("The function name \"" + fileFuncName + "\" does not match with the "
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
         try
         {
            inputArgs =
               theTextParser.SeparateBrackets(rhsParts[1], "()", ",");
            
            #if DBGLVL_FUNCTION_DEF > 1
            WriteStringArray("GmatFunction inputArgs", "", inputArgs);
            #endif
         }
         catch (BaseException &)
         {
            InterpreterException ex
               ("Invalid input argument list found in the GmatFunction file \"" +
                funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
            HandleError(ex, false);
            retval = false;
            break;
         }
         
         if (inputArgs.size() == 0)
         {
            InterpreterException ex
               ("The input argument list is empty in the GmatFunction file \"" +
                funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
            HandleError(ex, false);
            retval = false;
            break;
         }
         
         // check for duplicate input list
         #if DBGLVL_FUNCTION_DEF > 0
         MessageInterface::ShowMessage("   Check for duplicate input arguments\n");
         #endif
         if (inputArgs.size() > 1)
         {
            StringArray multiples;
            // check for duplicate input names
            for (UnsignedInt i=0; i<inputArgs.size(); i++)
            {
               for (UnsignedInt j=0; j<inputArgs.size(); j++)
               {
                  if (i == j)
                     continue;
                  
                  if (inputArgs[i] == inputArgs[j])
                     if (find(multiples.begin(), multiples.end(), inputArgs[i]) == multiples.end())
                        multiples.push_back(inputArgs[i]);
               }
            }
            
            if (multiples.size() > 0)
            {
               std::string errMsg = "Duplicate input of";
               
               for (UnsignedInt i=0; i<multiples.size(); i++)
                  errMsg = errMsg + " \"" + multiples[i] + "\"";
               
               InterpreterException ex
                  (errMsg + " found in the GmatFunction file \"" +
                   funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
               HandleError(ex, false);
               retval = false;
               break;
            }
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
   
   // if function definition has been validated, check if all outputs are declared
   #if DBGLVL_FUNCTION_DEF > 0
   MessageInterface::ShowMessage("   Check for output declaration\n");
   #endif
   if (retval && outputArgs.size() > 0)
   {
      std::string errMsg;
      IntegerArray rowCounts, colCounts;
      WrapperTypeArray outputTypes =
         GmatFileUtil::GetFunctionOutputTypes(&inStream, inputArgs, outputArgs, errMsg,
                                              rowCounts, colCounts);
      
      if (errMsg != "")
      {
         InterpreterException ex
            (errMsg + " found in the GmatFunction file \"" +
             funcPath + "\" referenced in \"" + function->GetName() + "\"\n");
         HandleError(ex, false);
         retval = false;
      }
      else
      {
         ((Function*)function)->SetOutputTypes(outputTypes, rowCounts, colCounts);
      }
   }
   
   inStream.close();
   
   
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
   WriteStringArray("parts", "", parts);
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
   WriteStringArray("lhsParts", "", lhsParts);
   WriteStringArray("rhsParts", "", rhsParts);
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
      WriteStringArray("function inputs", "", inputs);
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
      WriteStringArray("function outputs", "", outputs);
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


//------------------------------------------------------------------------------
// void ClearTempObjectNames()
//------------------------------------------------------------------------------
/*
 * Clears temporary object name array.
 * tempObjectNames is used for finding MatlabFunction names.
 * This method is called from the ScriptInterpreter::InterpretGmatFunction()
 */
//------------------------------------------------------------------------------
void Interpreter::ClearTempObjectNames()
{
   tempObjectNames.clear();
}


//------------------------------------------------------------------------------
// bool Interpreter::HandleMathTree(GmatCommand *cmd)
//------------------------------------------------------------------------------
bool Interpreter::HandleMathTree(GmatCommand *cmd)
{
   #ifdef DEBUG_MATH_TREE
   MessageInterface::ShowMessage
      ("Interpreter::HandleMathTree() '%s', It is a math equation\n",
       cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   Assignment *equation = (Assignment*)cmd;
   std::string lhs = equation->GetLHS();
   std::string rhs = equation->GetRHS();
   
   // Handle GmatFunction in math
   StringArray gmatFuns = equation->GetGmatFunctionNames();
   
   #ifdef DEBUG_MATH_TREE
   MessageInterface::ShowMessage("   Found %d GmatFunctions\n", gmatFuns.size());
   #endif
   
   for (UnsignedInt i=0; i<gmatFuns.size(); i++)
   {
      GmatBase *func = FindObject(gmatFuns[i]);
      Integer manage = 1;
      
      // Do not manage function if creating in function mode
      if (inFunctionMode)
         manage = 0;
      
      if (func == NULL)
         func = CreateObject("GmatFunction", gmatFuns[i], manage);
      
      #ifdef DEBUG_MATH_TREE
      MessageInterface::ShowMessage
         ("   Setting GmatFunction '%s'<%p> to equation<%p>\n",
          func->GetName().c_str(), func, equation);
      #endif
      
      equation->SetFunction((Function*)func);
   }
   
   #ifdef DEBUG_MATH_TREE
   MessageInterface::ShowMessage("Interpreter::HandleMathTree() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool HasFilenameTypeParameter(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Checks if the command has parameters with Gmat::FILENAME_TYPE.
 */
//------------------------------------------------------------------------------
bool Interpreter::HasFilenameTypeParameter(GmatCommand *cmd)
{
   Integer paramCount = cmd->GetParameterCount();
   bool fileTypeFound = false;
   for (Integer id = 0; id < paramCount; ++id)
   {
      if (!cmd->IsParameterReadOnly(id))
      {
         if (cmd->GetParameterType(id) == Gmat::FILENAME_TYPE)
         {
            fileTypeFound = true;
            break;
         }
      }
   }
   
   return fileTypeFound;
}



