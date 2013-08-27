//$Id$
//------------------------------------------------------------------------------
//                                  GmatCommand
//------------------------------------------------------------------------------
// GMAT: General MiHeaderssion Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Implementation code for the GmatCommand base class
 */
//------------------------------------------------------------------------------


#include "GmatCommand.hpp"       // class's header file
#include "CommandException.hpp"
#include "CoordinateConverter.hpp"
#include "MessageInterface.hpp"  // MessageInterface
#include "TimeSystemConverter.hpp"
#include "GmatDefaults.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "CalculationUtilities.hpp"
#include "StateConversionUtil.hpp"

#include <algorithm>             // for find()
#include <sstream>               // for command summary generation
#include <cmath>                 // for Cartesian to Keplerian conversion; 
                                 // remove it when the real conversions are
                                 // used.
#include <iostream>
#include <iomanip>

//#define DEBUG_COMMAND_DEALLOCATION
//#define DEBUG_COMMAND_SUMMARY_LIST
//#define DEBUG_COMMAND_INIT 1
//#define DEBUG_BUILD_CMD_SUMMARY 1
//#define DEBUG_COMMAND_APPEND 1
//#define DEBUG_COMMAND_INSERT 1
//#define DEBUG_COMMAND_REMOVE 1
//#define DEBUG_COMMAND_SET 1
//#define DEBUG_RUN_COMPLETE 1
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_FIND_OBJECT
//#define DEBUG_SEPARATE
//#define DEBUG_GEN_STRING 1
//#define DEBUG_IS_FUNCTION
//#define DEBUG_INTERPRET_PREFACE
//#define DEBUG_CMD_CALLING_FUNCTION
//#define DEBUG_COMMAND_SUMMARY_STATE
//#define DEBUG_COMMAND_SUMMARY_REF_DATA
//#define DEBUG_COMMAND_SUMMARY_TYPE
//#define DEBUG_MISSION_SUMMARY
//#define DEBUG_SUMMARY_STRINGS
//#define DEBUG_DEFSTR
//#define DEBUG_CMD_SUMMARY

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//------------------------------------------------------------------------------
//  static members
//------------------------------------------------------------------------------
const std::string 
   GmatCommand::PARAMETER_TEXT[GmatCommandParamCount - GmatBaseParamCount] =
   {
      "Comment",
      "Summary",
      "MissionSummary"
   };


const Gmat::ParameterType 
   GmatCommand::PARAMETER_TYPE[GmatCommandParamCount - GmatBaseParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE
   };

const Integer GmatCommand::MAX_NUM_TANKS = 20;

const std::string GmatCommand::MISSION_CHANGE_MESSAGE =
         "******  Changes made to the mission will not be reflected ******\n"
         "******  in the data displayed until the mission is rerun  ******\n\n";

Integer GmatCommand::satEpochID = -1;
Integer GmatCommand::satCdID;
Integer GmatCommand::satDragAreaID;
Integer GmatCommand::satCrID;
Integer GmatCommand::satSRPAreaID;
Integer GmatCommand::satTankID;
Integer GmatCommand::satThrusterID;
Integer GmatCommand::satDryMassID;
Integer GmatCommand::satTotalMassID;


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatCommand(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Constructs GmatCommand core structures (default constructor).
 *
 * @param typeStr The command's scriptable name
 */
//------------------------------------------------------------------------------
GmatCommand::GmatCommand(const std::string &typeStr) :
   GmatBase             (Gmat::COMMAND, typeStr),
   currentFunction      (NULL),
   callingFunction      (NULL),
   next                 (NULL),
   previous             (NULL),
   level                (-1),   // Not set
   objectMap            (NULL),
   globalObjectMap      (NULL),
   solarSys             (NULL),
   triggerManagers      (NULL),
   internalCoordSys     (NULL),
   forces               (NULL),
   events               (NULL),
   publisher            (NULL),
   streamID             (-1),
   depthChange          (0),
   commandChangedState  (false),
   commandSummary       (""),
   summaryCoordSysName  ("EarthMJ2000Eq"),
   summaryCoordSys      (NULL),
   summaryForEntireMission (false),
   missionPhysicsBasedOnly (false),
   physicsBasedCommand  (false),
   includeInSummary     (true),
   summaryName          ("Unnamed"),
   commandChanged       (false),
   cloneCount           (0),
   currentRunState      (Gmat::RUNNING),  // RUNNING means just treat normally
   epochData            (NULL),
   stateData            (NULL),
   parmData             (NULL),
   fuelMassData         (NULL),
   satsInMaps           (0)
{
   generatingString = "";
   parameterCount = GmatCommandParamCount;
   objectTypes.push_back(Gmat::COMMAND);
   objectTypeNames.push_back("GmatCommand");
   objectTypeNames.push_back(typeStr);
   
   commandNameList.push_back(typeStr);
   parser.Initialize(commandNameList);
   
   BuildCommandSummary(false);
}


//------------------------------------------------------------------------------
//  ~GmatCommand()
//------------------------------------------------------------------------------
/**
 * Destructs GmatCommand core structures.
 *
 * The GmatCommand destructor calls the destructor for the next GmatCommand in
 * the sequence.  Therefore, when a GmatCommand sequence is cleared, all the
 * Sandbox needs to do is call the destructor on the first GmatCommand in the
 * sequence, and the remaining GmatCommands are all destructed.
 *
 * @note The branching GmatCommands will also destroy all of the GmatCommands in
 *       the subbranch(es)
 */
//------------------------------------------------------------------------------
GmatCommand::~GmatCommand()
{
   #ifdef DEBUG_COMMAND_DEALLOCATION
      std::string nextStr = "NULL";
      if (next)
         nextStr = next->GetTypeName();

      ShowCommand("", "~GmatCommand() this=", this, ", next=", next);
      //MessageInterface::ShowMessage
      //   ("In GmatCommand::~GmatCommand() this=%s, next=%s\n",
      //    this->GetTypeName().c_str(), nextStr.c_str());
   #endif
      
   // We need to clean up summary data, so removed return if BranchEnd
   if (!this->IsOfType("BranchEnd"))
   {      
      // Delete the subsequent GmatCommands
      if (next) {
         #ifdef DEBUG_COMMAND_DEALLOCATION
         MessageInterface::ShowMessage("   Deleting (%p)%s\n", next, 
                                       next->GetTypeName().c_str());
         #endif
         
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (next, next->GetTypeName(), this->GetTypeName() +
             "::~GmatCommand() deleting child command");
         #endif
         delete next;   
      }
   }
   
   // Clean up the summary buffers
   if (epochData)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (epochData, "epochData", this->GetTypeName() +
          "::~GmatCommand() deleting epochData");
      #endif
      delete [] epochData;
      epochData = NULL;
   }
   if (stateData)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (stateData, "stateData", this->GetTypeName() +
          "::~GmatCommand() deleting stateData");
      #endif
      delete [] stateData;
      stateData = NULL;
   }
   if (parmData)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (parmData, "parmData", this->GetTypeName() +
          "::~GmatCommand() deleting parmData");
      #endif
      delete [] parmData;
      parmData = NULL;
   }
   if (fuelMassData)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (fuelMassData, "fuelMassData", this->GetTypeName() +
          "::~GmatCommand() deleting fuelMassData");
      #endif
      delete [] fuelMassData;
      fuelMassData = NULL;
   }
   tankNames.clear();
}


//------------------------------------------------------------------------------
//  GmatCommand(const GmatCommand &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * The copy constructor for GmatCommands copies object lists and associations
 * from the "original" GmatCommand, c, but not its navigation parameters (next, 
 * level), nor does it initialize the GmatCommand.
 *
 * @param c GmatCommand object whose values to use to make "this" copy.
 */
//------------------------------------------------------------------------------
GmatCommand::GmatCommand(const GmatCommand &c) :
   GmatBase             (c),
   association          (c.association),
   objects              (c.objects),
   currentFunction      (c.currentFunction),
   callingFunction      (c.callingFunction),
   next                 (NULL),
   previous             (NULL),
   level                (-1),   // Not set
   objectMap            (c.objectMap),
   globalObjectMap      (c.globalObjectMap),
   solarSys             (c.solarSys),
   triggerManagers      (c.triggerManagers),
   internalCoordSys     (c.internalCoordSys),
   forces               (c.forces),
   events               (c.events),
   publisher            (c.publisher),
   streamID             (c.streamID),
   depthChange          (c.depthChange),
   commandChangedState  (c.commandChangedState),
   commandSummary       (c.commandSummary),
   summaryCoordSysName  (c.summaryCoordSysName),
   summaryCoordSys      (NULL),
   summaryForEntireMission (c.summaryForEntireMission),
   missionPhysicsBasedOnly (c.missionPhysicsBasedOnly),
   physicsBasedCommand  (c.physicsBasedCommand),
   includeInSummary     (c.includeInSummary),
   summaryName          (c.summaryName),
   commandChanged       (c.commandChanged),
   settables            (c.settables),
   cloneCount           (0),
   currentRunState      (c.currentRunState),
   epochData            (NULL),
   stateData            (NULL),
   parmData             (NULL),
   fuelMassData         (NULL),
   satsInMaps           (0)
{
   generatingString = c.generatingString;

   tankNames.clear();
}


//------------------------------------------------------------------------------
//  GmatCommand operator=(const GmatCommand &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * The assignment operator for GmatCommands copies the object lists and
 * associations from the "other" GmatCommand, c, but not its navigation
 * parameters (next, level, depthChange, commandChangedState).   This operator
 * clears all object associations, and leaves "this" object in an uninitialized
 * state.
 *
 * @param c GmatCommand object whose values to use to set "this" GmatCommand
 *          object.
 *
 * @return GmatCommand object.
 */
//------------------------------------------------------------------------------
GmatCommand& GmatCommand::operator=(const GmatCommand &c)
{
   if (&c == this)
      return *this;

   GmatBase::operator=(c);
   isInitialized         = false;
   currentFunction     = c.currentFunction;
   callingFunction     = c.callingFunction;
   objects.clear();
   association.clear();
   ClearObjects();             // Drop any previously set object pointers
   
   objects             = c.objects;
   association         = c.association;
   
   objectMap           = c.objectMap;
   globalObjectMap     = c.globalObjectMap;
   solarSys            = c.solarSys;
   internalCoordSys    = c.internalCoordSys;
   forces              = c.forces;
   events              = c.events;
   publisher           = c.publisher;
   generatingString    = c.generatingString;
   streamID            = c.streamID;
//   comment             = c.comment;
   commandChangedState = c.commandChangedState;
   commandSummary      = c.commandSummary;
   summaryCoordSysName = c.summaryCoordSysName;
   summaryCoordSys     = c.summaryCoordSys;
   summaryForEntireMission = c.summaryForEntireMission;
   missionPhysicsBasedOnly = c.missionPhysicsBasedOnly;
   physicsBasedCommand = c.physicsBasedCommand;
   includeInSummary    = c.includeInSummary;
   summaryName         = c.summaryName;
   commandChanged      = c.commandChanged;
   cloneCount          = 0;
   currentRunState     = c.currentRunState;
   settables           = c.settables;
   
   epochData           = NULL;
   stateData           = NULL;
   parmData            = NULL;
   fuelMassData        = NULL;
   tankNames.clear();
   
   isInitialized         = false;

   return *this;
}


//------------------------------------------------------------------------------
//  void SetGeneratingString(const std::string &gs)
//------------------------------------------------------------------------------
/**
 * Method used to store the string that was parsed to build this GmatCommand.
 *
 * This method is used to store the GmatCommand string so that the script can be
 * written to a file without inverting the steps used to set up the internal
 * object data.
 *
 * @param <gs> The script line that was interpreted to define this GmatCommand.
 */
//------------------------------------------------------------------------------
void GmatCommand::SetGeneratingString(const std::string &gs)
{
   // Drop the leading white space
   UnsignedInt start = 0, end;
   while ((start < gs.length()) && (gs[start] == ' '))
      ++start;
   generatingString = gs.substr(start);
   
   end = gs.length()-1;
   while ((end > 0) && (gs[end] == ' '))
      --end;

   generatingString = gs.substr(0, end+1);
}

//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString()
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
 * @param mode    Specifies the type of serialization requested. (Not yet used
 *                in commands)
 * @param prefix  Optional prefix appended to the object's name.  (Not yet used
 *                in commands)
 * @param useName Name that replaces the object's name.  (Not yet used in
 *                commands)
 *
 * @return The script line that was interpreted to define this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& GmatCommand::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   static std::string empty;
   if (generatingString == "") {
      if (typeName == "NoOp")
         return generatingString;
      empty = "% Generating string not set for " + typeName + " command.";
      return empty;
   }
   
   if (mode == Gmat::NO_COMMENTS)
   {
      InsertCommandName(generatingString);      
	  return generatingString;
   }
   
   
   std::string commentLine = GetCommentLine();
   std::string inlineComment = GetInlineComment();
   
   #if DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("GmatCommand::GetGeneratingString() entered, generatingString =\n<%s>\n",
       generatingString.c_str());
   MessageInterface::ShowMessage
      ("   commentLine = <%s>\n   inlineComment = <%s>\n",
       commentLine.c_str(), inlineComment.c_str());   
   #endif
   
   // write preface comment
   if (showPrefaceComment)
   {
      std::stringstream gen;
      if (commentLine != "")
      {
         TextParser tp;
         StringArray textArray = tp.DecomposeBlock(commentLine);
         
         if (textArray.size() > 0 && textArray[0] != "")
         {
            // handle multiple comment lines
            for (UnsignedInt i=0; i<textArray.size(); i++)
            {
               gen << prefix << textArray[i];
               if (textArray[i].find("\n") == commentLine.npos &&
                   textArray[i].find("\r") == commentLine.npos)
                  gen << "\n";
            }
         }
      }
      
      InsertCommandName(generatingString);      
      generatingString = gen.str() + generatingString;
   }
   
   
   if (showInlineComment)
      if (inlineComment != "")
         generatingString = generatingString + inlineComment;
   
   #if DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("===> return\n<%s>\n\n", generatingString.c_str());
   #endif
   
   return generatingString;
}


//------------------------------------------------------------------------------
// void SetCurrentFunction(Function *function)
//------------------------------------------------------------------------------
/**
 * This method sets the current function pointer
 *
 * @param function The function that is getting set
 *
 * @return
 */
//------------------------------------------------------------------------------
void GmatCommand::SetCurrentFunction(Function *function)
{
   currentFunction = function;
}


//------------------------------------------------------------------------------
// Function* GetCurrentFunction()
//------------------------------------------------------------------------------
/**
 * This method retrieves the current function pointer
 *
 * @return The current function pointer
 */
//------------------------------------------------------------------------------
Function* GmatCommand::GetCurrentFunction()
{
   return currentFunction;
}


//------------------------------------------------------------------------------
// void SetCallingFunction(FunctionManager *fm)
//------------------------------------------------------------------------------
/**
 * Sets the calling function pointer
 *
 * @param fm
 */
//------------------------------------------------------------------------------
void GmatCommand::SetCallingFunction(FunctionManager *fm)
{
   #ifdef DEBUG_CMD_CALLING_FUNCTION
      MessageInterface::ShowMessage(
            "NOW setting calling function on command of type %s\n",
            (GetTypeName()).c_str());
   #endif
   callingFunction = fm;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper* toWrapper,
//          const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Adds an element wrapper to the collection of wrappers
 *
 * @param toWrapper The element wrapper
 * @param withName The name of the wrapper
 *
 * @return true on success, false on failure
 *
 * @note The default implementation does nothing and returns false
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetElementWrapper(ElementWrapper* toWrapper,
                                    const std::string &withName)
{
   return false;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
/**
 * This method clears the list of wrappers
 *
 * @note This default implementation does nothing
 */
//------------------------------------------------------------------------------
void GmatCommand::ClearWrappers()
{
}


//------------------------------------------------------------------------------
// void CheckDataType(ElementWrapper* forWrapper,
//                    Gmat::ParameterType needType,
//                    const std::string &cmdName,
//                    bool ignoreUnsetReference)
//------------------------------------------------------------------------------
/**
 * This method checks the data type for a wrapper against the needed type.
 *
 * The method validates the data type, and throws an exception if there is a
 * type mismatch.
 *
 * @param forWrapper The wrapper that is being checked
 * @param needType The data type that is needed to use the wrapper
 * @param cmdName The name of the command that is being checked
 * @param ignoreUnsetReference Flag used to control exceptions when the wrapper
 *                             object is not set
 */
//------------------------------------------------------------------------------
void GmatCommand::CheckDataType(ElementWrapper* forWrapper,
                                Gmat::ParameterType needType,
                                const std::string &cmdName,
                                bool ignoreUnsetReference)
{
   if (forWrapper == NULL)
   {
      std::string cmdEx = "Reference object not set for command " + 
                          cmdName;
      cmdEx += ".\n";
      throw CommandException(cmdEx);
   }
   bool typeOK = true;
   Gmat::ParameterType baseType;
   std::string         baseStr;
   std::string         desc = forWrapper->GetDescription();
   try
   {
      baseType = forWrapper->GetDataType();
      baseStr  = PARAM_TYPE_STRING[baseType];
      if ((baseType != needType) && 
          !((needType == Gmat::REAL_TYPE) && (baseType == Gmat::INTEGER_TYPE)))
         typeOK = false;
   }
   catch (BaseException &be)
   {
      // Use exception to remove Visual C++ warning
      be.GetMessageType();
      // will need to check data type of object property 
      // wrappers on initialization
      if (!ignoreUnsetReference)
      {
         std::string errmsg = "Reference not set for \"" + desc;
         errmsg += "\", cannot check for correct data type.";
         throw CommandException(errmsg);
      }
   }
   
   if (!typeOK)
   {
      throw CommandException("A value of \"" + desc + "\" of base type \"" +
                  baseStr + "\" on command \"" + cmdName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Object Property (Real), Real Number, Variable, "
                  "Array Element, or Parameter ]. ");
   }
}


//------------------------------------------------------------------------------
//  bool SetObject(const std::string &name, const Gmat::ObjectType type,
//                 const std::string &associate, 
//                 const Gmat::ObjectType associateType)
//------------------------------------------------------------------------------
/**
 * Stores the object names and types required for the GmatCommand.
 *
 * Objects associated with a GmatCommand are not necessarily available at the
 * time the GmatCommand is parsed from the script.  This GmatCommand builds the
 * data store of object names that will be used to execute the GmatCommand when
 * the script is run.
 * 
 * @param name The name of the object.
 * @param type The (enumerated) base type assigned to the object.
 * @param associate Optional second object associated with the named object.
 *                  Defaults to the empty string ("").
 * @param associateType Type for the second object; defaults to
 *                      Gmat::UNKNOWN_OBJECT.
 *
 * @return true object is stored successfully, or it it is already registered
 *         with the same type, false otherwise.
 *
 * @note associate and associateType are provided to allow derived classes to
 *       override this method for GmatCommands that link two types of objects
 *       together -- for example, the Propagate GmatCommand links a propagator
 *       setup to a spacecraft.  These parameters are not used in this default
 *       implementation.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetObject(const std::string &name,
                        const Gmat::ObjectType type,
                        const std::string &associate,
                        const Gmat::ObjectType associateType)
{
   // Check to see if it is already in the object list
   StringArray::iterator item = std::find(objects.begin(), objects.end(), name);
   if (item != objects.end()) 
   {
      if (type != association[*item])  // make sure the types match
         return false;
   }
   else
   {
      // Add it to the lists
      objects.push_back(name);
      association[name] = type; 
   }
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* GetObject(const Gmat::ObjectType type, const std::string objName)
//------------------------------------------------------------------------------
/**
 * Retrieves the objects used by the GmatCommand.
 *
 * Objects associated with a GmatCommand are not necessarily available at the
 * time the GmatCommand is parsed from the script.  This GmatCommand builds the
 * data store of object names that will be used to execute the GmatCommand when
 * the script is run.
 * 
 * @param type    The (enumerated) base type assigned to the object.
 * @param objName The name of the object.
 *
 * @return A pointer to the requested object, obtained from the Configuration 
 *         Manager if it is a configured object, or from the local pointer if it
 *         is an internal object.
 */
//------------------------------------------------------------------------------
GmatBase* GmatCommand::GetGmatObject(const Gmat::ObjectType type, 
                                  const std::string objName)
{
   return NULL;
}


//------------------------------------------------------------------------------
//  bool SetObject(GmatBase *obj, const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Stores pointers to objects required exclusively for the current GmatCommand.
 *
 * Some GmatCommands require helper objects to execute successfully -- for
 * example, the Propagate GmatCommand uses a StoppingCondition object to
 * evaluate when the GmatCommand finishes execution.  Use this method to assign
 * these GmatCommand specific objects.
 *
 * @param obj Pointer to the object.
 * @param type The (enumerated) base type assigned to the object.
 *
 * @return true when object is stored successfully, false otherwise.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
   return false;
}


//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local solar system for the GmatCommand
 * 
 * @param ss Pointer to the local solar system
 */
//------------------------------------------------------------------------------
void GmatCommand::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_COMMAND_SET
   MessageInterface::ShowMessage
      ("GmatCommand::SetSolarSystem() entered, ss=<%p>\n", ss);
   #endif
   
   solarSys = ss;
}


//------------------------------------------------------------------------------
// void SetTriggerManagers(std::vector<TriggerManager*> *trigs)
//------------------------------------------------------------------------------
/**
 * Sets the trigger manager pointers
 *
 * @param trigs A vector of TriggerManager pointers passed in from the local
 *              Sandbox.
 */
//------------------------------------------------------------------------------
void GmatCommand::SetTriggerManagers(std::vector<TriggerManager*> *trigs)
{
   triggerManagers = trigs;
}

//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * Sets internal coordinate system which state is propagated.
 *
 * @note Current Internal coordinate system used is MJ2000Eq of j2000body as
 *       origin and current j2000body is Earth.
 */
//------------------------------------------------------------------------------
void GmatCommand::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #ifdef DEBUG_COMMAND_SET
   MessageInterface::ShowMessage
      ("GmatCommand::SetInternalCoordSystem() entered, cs=<%p>\n", cs);
   #endif
   
   internalCoordSys = cs;
}


//------------------------------------------------------------------------------
// void SetupSummary(const std::string &csName, bool entireMission,
//       bool physicsOnly)
//------------------------------------------------------------------------------
/*
 * Sets the name of the coordinate system and flags to use for the Command
 * Summary display.
 *
 * @param csName Name of the coordinate system used for the summary
 * @param entireMission Flag indicating if the summary is being generated for
 *          the entire mission (default: false)
 * @param physicsOnly Flag indicating if the summary is generated for only
 *          physics affecting commands
 */
//------------------------------------------------------------------------------
void GmatCommand::SetupSummary(const std::string &csName, bool entireMission,
      bool physicsOnly)
{
   #ifdef DEBUG_COMMAND_SET
   MessageInterface::ShowMessage
      ("GmatCommand::SetupSummary() entered, cs=<%s>\n", csName.c_str());
   #endif

   summaryCoordSysName     = csName;
   summaryForEntireMission = entireMission;
   missionPhysicsBasedOnly = physicsOnly;
}

//------------------------------------------------------------------------------
// void SetSummaryName(const std::string &sumName)
//------------------------------------------------------------------------------
/**
 * Sets the name of the command summary
 *
 * @param sumName The name
 */
//------------------------------------------------------------------------------
void GmatCommand::SetSummaryName(const std::string &sumName)
{
   summaryName = sumName;
   #ifdef DEBUG_CMD_SUMMARY
      MessageInterface::ShowMessage("Setting summary name for command \"%s\" (type %s) to \"%s\"\n",
            instanceName.c_str(), typeName.c_str(), summaryName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// std::string GetSummaryName()
//------------------------------------------------------------------------------
/**
 * Retrieves the command summary name
 *
 * @return The name
 */
//------------------------------------------------------------------------------
std::string GmatCommand::GetSummaryName()
{
   return summaryName;
}


//------------------------------------------------------------------------------
//  void SetObjectMap(std::map<std::string, GmatBase *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local asset store used by the GmatCommand
 * 
 * @param map Pointer to the local object map
 */
//------------------------------------------------------------------------------
void GmatCommand::SetObjectMap(std::map<std::string, GmatBase *> *map)
{
   objectMap = map;
}


//------------------------------------------------------------------------------
// ObjectMap* GetObjectMap()
//------------------------------------------------------------------------------
/**
 * Retrieves the object map
 *
 * @return The map
 */
//------------------------------------------------------------------------------
ObjectMap* GmatCommand::GetObjectMap()
{
   return objectMap;
}


//------------------------------------------------------------------------------
//  void SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the global asset store used by the GmatCommand
 * 
 * @param map Pointer to the global object map
 */
//------------------------------------------------------------------------------
void GmatCommand::SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
{
   globalObjectMap = map;
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
 * Passes the transient force vector into the commands that need them
 * 
 * The transient force vector is a set of models used in GMAT's ODEModel for
 * affects that are turned on and off over the course of a mission.  An example
 * of a transient force is a finite burn, which is toggled by the
 * BeginFiniteBurn and EndFiniteBurn commands.  These components are only used
 * by commands that need them.  Typical usage is found in the propagation
 * enabled commands.
 *
 * @param tf The vector of transient forces
 */
//------------------------------------------------------------------------------
void GmatCommand::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   forces = tf;
}


//------------------------------------------------------------------------------
//  void SetEventLocators(std::vector<EventLocator*> *els)
//------------------------------------------------------------------------------
/**
 * Passes the EventLocator collection into the commands that need them
 *
 * @param tf The vector of EventLocators
 *
 * @note The default behavior in the GmatCommands is to ignore the vector.
 */
//------------------------------------------------------------------------------
void GmatCommand::SetEventLocators(std::vector<EventLocator*> *els)
{
   events = els;
}


//------------------------------------------------------------------------------
//  void SetPublisher(Publisher *p)
//------------------------------------------------------------------------------
/**
 * Sets the Publisher used for data generated by the GmatCommand
 * 
 * @param p Pointer to the Publisher
 */
//------------------------------------------------------------------------------
void GmatCommand::SetPublisher(Publisher *p)
{
   publisher = p;
}


//------------------------------------------------------------------------------
//  Publisher* GetPublisher()
//------------------------------------------------------------------------------
/**
 * Returns the Publisher used for data generated by the GmatCommand
 *
 * @return The Publisher
 */
//------------------------------------------------------------------------------
Publisher* GmatCommand::GetPublisher()
{
   return publisher;
}


//------------------------------------------------------------------------------
// const StringArray& GetObjectList()
//------------------------------------------------------------------------------
/**
 * Returns a list of objects referenced by the command, so that the command can
 * be validated as only referencing existing objects.
 *
 * Derived classes can override this method to build different lists, or to add
 * to the objects list.
 *
 * @return The list of objects
 */
//------------------------------------------------------------------------------
const StringArray& GmatCommand::GetObjectList()
{
   return objects;
}

//------------------------------------------------------------------------------
// bool AcceptsObjectType(Gmat::OBJECT_TYPE theType)
//------------------------------------------------------------------------------
/**
 * Checks to see if a specific type is supported by the command instance
 *
 * This method is used to validate at build time that a command instance can
 * use object types that may be allowed or disallowed based in the command
 * configuration.  An example of a disallowed type detected by this call are is
 * a Propagate command containing a Formation object that also propagates the
 * STM, a configuration that is not currently allowed in GMAT.
 *
 * @param theType The object type being checked
 *
 * @return true if the type is accepted, false if not
 */
//------------------------------------------------------------------------------
bool GmatCommand::AcceptsObjectType(Gmat::ObjectType theType)
{
   return true;
}

//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
/**
 * Performs internal validation of the command
 *
 * This method is called during the final pass of script reading to verify that
 * the command has internal consistency.
 *
 * @return true if the command is internally consistent, false if not.
 */
//------------------------------------------------------------------------------
bool GmatCommand::Validate()
{
   return GmatBase::Validate();
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param id Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string GmatCommand::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < GmatCommandParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
      
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param str Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer GmatCommand::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < GmatCommandParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType GmatCommand::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < GmatCommandParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
      
   return GmatBase::GetParameterType(id);
}



//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param id Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string GmatCommand::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 * 
 * For GmatCommands, this is a bit of a misnomer because the comment field is 
 * not read-only.  It is accessed only in special circumstances, though -- 
 * comment lines come before the command in the script -- and therefore gets
 * special treatment in the Interpreters.
 *
 * @param id Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//------------------------------------------------------------------------------
bool GmatCommand::IsParameterReadOnly(const Integer id) const
{
   if ((id >= GmatBaseParamCount) && (id < GmatCommandParamCount))
      return true;

   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 * 
 * @param label Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//------------------------------------------------------------------------------
bool GmatCommand::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
std::string GmatCommand::GetStringParameter(const Integer id) const
{
   if (id == SUMMARY)
   {
      // This call is not const, so need to break const-ness here:
      ((GmatCommand*)this)->BuildCommandSummaryString();
      #ifdef DEBUG_SUMMARY_STRINGS
         MessageInterface::ShowMessage("Command Summary:\n%s\n",
               commandSummary.c_str());
      #endif
      return commandSummary;
   }
   
   if (id == MISSION_SUMMARY)
   {
      // This call is not const, so need to break const-ness here:
      std::string missionSummary =
         ((GmatCommand*)this)->BuildMissionSummaryString(this);
      #ifdef DEBUG_SUMMARY_STRINGS
         MessageInterface::ShowMessage("Mission Summary:\n%s\n",
               missionSummary.c_str());
      #endif
      missionSummary = MISSION_CHANGE_MESSAGE + missionSummary;
      return missionSummary;
   }
         
   return GmatBase::GetStringParameter(id);
}

   
//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id    The integer ID for the parameter.
 * @param index Array index for parameters.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
std::string GmatCommand::GetStringParameter(const Integer id, 
                                            const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}

   
//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param id    Integer ID of the parameter.
 * @param value New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetStringParameter(const Integer id, const std::string &value)
{
//   if (id == COMMENT)
//   {
//      comment = value;
//      return true;
//   }
         
   if (id == SUMMARY)
   {
      return false;
   }
         
   if (id == MISSION_SUMMARY)
   {
      return false;
   }
         
   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const Real value, 
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param id    Integer ID of the parameter.
 * @param value New value for the parameter.
 * @param index array index for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetStringParameter(const Integer id, const std::string &value,
                                     const Integer index)
{
   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
std::string GmatCommand::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label,
//                                 const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index array index for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
std::string GmatCommand::GetStringParameter(const std::string &label,
                                            const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value New value for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetStringParameter(const std::string &label, 
                                     const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value New value for the parameter.
 * @param index array index for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetStringParameter(const std::string &label, 
                                     const std::string &value,
                                     const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
//  bool SetCondition(const std::string &lhs, 
//                    const std::string &operation, 
//                    const std::string &rhs, 
//                    Integer atIndex)
//------------------------------------------------------------------------------
/**
 * Sets a condition for the command, at index atIndex (-999 to add to the end
 * of the list).
 * 
 * @param lhs       string for the left hand side of the condition
 * @param operation string for the operator
 * @param rhs       string for the right hand side of the condition
 * @param atIndex   where in the list to place the condition (-999 means to
 *                    add it to the end of the list (default))
 *
 * @return true if the condition was set, false if not.  The default
 *         (implemented here) returns false.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetCondition(const std::string &lhs, 
                               const std::string &operation, 
                               const std::string &rhs, 
                               Integer atIndex)
{
   return false;
}

//------------------------------------------------------------------------------
//  bool SetConditionOperator(const std::string &op, 
//                            Integer atIndex)
//------------------------------------------------------------------------------
/**
 * Sets a logical operator for the command, at index atIndex (-999 to add to the
 * end of the list) - this is the operator connecting conditions, when there are
 * multiple conditions.
 * 
 * @param op      string for the logical operator
 * @param atIndex where in the list to place the logical operator (-999 means
 *                to add it to the end of the list (default))
 *
 * @return true if the operator was set, false if not.  The default
 *         (implemented here) returns false.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetConditionOperator(const std::string &op, 
                                       Integer atIndex)
{
   return false;
}

//------------------------------------------------------------------------------
//  bool RemoveCondition(Integer atIndex)
//------------------------------------------------------------------------------
/**
 * Removes the condition for the command, at index atIndex.
 * 
 * @param atIndex where in the list to remove the condition from.
 *
 * @return true if the condition was removed, false if not.  The default
 *         (implemented here) returns false.
 */
//------------------------------------------------------------------------------
bool GmatCommand::RemoveCondition(Integer atIndex)
{
   return false;
}

//------------------------------------------------------------------------------
//  bool RemoveConditionOperator(Integer atIndex)
//------------------------------------------------------------------------------
/**
 * Removes the logical operator for the command, at index atIndex.
 * 
 * @param atIndex where in the list to remove the logical operator from.
 *
 * @return true if the operator was removed, false if not.  The default
 *         (implemented here) returns false.
 */
//------------------------------------------------------------------------------
bool GmatCommand::RemoveConditionOperator(Integer atIndex)
{
   return false;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the GmatCommand structures at the start of a run.
 *
 * This method is called by the Sandbox when it receives a run GmatCommand from
 * the Moderator.  This method calls AssignObjects to set the local object
 * pointers prior to execution.
 *
 * Derived GmatCommand objects can use this method to set up any internal data 
 * structures needed to run the GmatCommand.  If this method is overridden, the
 * base class method provided here should be called to ensure that AssignObjects
 * is called at the start of the run.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool GmatCommand::Initialize()
{
   #if DEBUG_COMMAND_INIT
   MessageInterface::ShowMessage
      ("GmatCommand::Initialize() %s entering\n   epochData=%p, stateData=%p, "
       "parmData=%p, fuelMassData = %p, satVector.size()=%d\n", GetTypeName().c_str(), epochData,
       stateData, parmData, fuelMassData, satVector.size());
   #endif
   
   // Check to be sure the basic infrastructure is in place
   if (objectMap == NULL)
   {
      std::string errorstr("Object map has not been initialized for ");
      errorstr += GetTypeName();
      throw CommandException(errorstr);
   }
   
   if (globalObjectMap == NULL)
   {
      std::string errorstr("Global object map has not been initialized for ");
      errorstr += GetTypeName();
      throw CommandException(errorstr);
   }
   
   if (solarSys == NULL)
   {
      std::string errorstr("Solar system has not been initialized for ");
      errorstr += GetTypeName();
      throw CommandException(errorstr);
   }
   
   isInitialized = AssignObjects();
   if (publisher == NULL)
      publisher = Publisher::Instance();
   
   if (epochData != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (epochData, "epochData", this->GetTypeName() + " deleting epochData");
      #endif
      delete [] epochData;
      epochData = NULL;
   }
   
   if (stateData != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (stateData, "stateData", this->GetTypeName() + " deleting stateData");
      #endif
      delete [] stateData;
      stateData = NULL;
   }
   
   if (parmData != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (parmData, "parmData", this->GetTypeName() + " deleting parmData");
      #endif
      delete [] parmData;
      parmData = NULL;
   }
   
   if (fuelMassData != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (fuelMassData, "fuelMassData", this->GetTypeName() + " deleting fuelMassData");
      #endif
      delete [] fuelMassData;
      fuelMassData = NULL;
   }

   tankNames.clear();

   satVector.clear();
   satsInMaps = 0;
   
   #if DEBUG_COMMAND_INIT
   MessageInterface::ShowMessage
      ("GmatCommand::Initialize() %s leaving\n   epochData=%p, stateData=%p, "
       "parmData=%p, fuelMassData = %p, satVector.size()=%d\n", GetTypeName().c_str(), epochData,
       stateData, parmData, fuelMassData, satVector.size());
   #endif
   
   return isInitialized;
}


//------------------------------------------------------------------------------
//  GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Accesses the next GmatCommand to be executed in the GmatCommand sequence
 * 
 * @return Pointer to the next GmatCommand.  Commands that need to be reentrant
 *       override this method and return their this pointer as needed.  Branch
 *       commands override this method to manage branching.
 */
//------------------------------------------------------------------------------
GmatCommand* GmatCommand::GetNext()
{
   return next;
}


//------------------------------------------------------------------------------
//  GmatCommand* GetPrevious()
//------------------------------------------------------------------------------
/**
 * Accesses the previous GmatCommand in the GmatCommand sequence
 * 
 * @return Pointer to the previous GmatCommand
 */
//------------------------------------------------------------------------------
GmatCommand* GmatCommand::GetPrevious()
{
   return previous;
}


//------------------------------------------------------------------------------
// bool GmatCommand::ForceSetNext(GmatCommand *toCmd)
//------------------------------------------------------------------------------
/**
 * Forces the next command pointer to be the input value
 *
 * This method overrides the linked list handling methods in the Mission Control
 * Sequence, replacing the next pointer with the input value.
 *
 * This method should only be used in special circumstances.  Usage can lead to
 * serious memory leaks
 *
 * @param toCmd The GmatCommand pointer
 *
 * @return true
 */
//------------------------------------------------------------------------------
bool GmatCommand::ForceSetNext(GmatCommand *toCmd) // dangerous!
{
   next = toCmd;
   return true;
}


//------------------------------------------------------------------------------
// bool ForceSetPrevious(GmatCommand *toCmd)
//------------------------------------------------------------------------------
/**
 * Forces the previous command pointer to be the input value
 *
 * This method overrides the linked list handling methods in the Mission Control
 * Sequence, replacing the previous pointer with the input value.
 *
 * This method should only be used in special circumstances.  Usage can lead to
 * serious memory leaks
 *
 * @param toCmd The GmatCommand pointer
 *
 * @return true
 */
//------------------------------------------------------------------------------
bool GmatCommand::ForceSetPrevious(GmatCommand *toCmd) // dangerous!
{
   previous = toCmd;
   return true;
}


//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds GmatCommand at the end of the GmatCommand sequence
 * 
 * @param cmd Pointer to GmatCommand that is added at the end
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool GmatCommand::Append(GmatCommand *cmd)
{
   #if DEBUG_COMMAND_APPEND > 1
   ShowCommand("GmatCommand::", "Append() this = ", this, " next = ", next);
   ShowCommand("GmatCommand::", "Append() cmd = ", cmd);
   #endif
   
   if (cmd == this)
      throw CommandException("Attempting to add GmatCommand already in list");
   
   if (next)
   {
      #ifdef DEBUG_COMMAND_APPEND
      ShowCommand("GmatCommand::", " appending ", cmd, " to ", next);
      #endif
      next->Append(cmd);
   }
   else
   {
      // Always set the command changed flag when a command is added to the list
      commandChanged = true;
      
      #ifdef DEBUG_COMMAND_APPEND
      ShowCommand("GmatCommand::", " Setting next of ", this, " to ", cmd);
      #endif
      
      next = cmd;
      
      // Do not set previous command if this is branch end 
      if (!this->IsOfType("BranchEnd"))
      {
         #ifdef DEBUG_COMMAND_APPEND
         ShowCommand("GmatCommand::", " Setting previous of ", cmd, " to ", this);
         #endif
         
         cmd->previous = this;
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
//  bool Insert(GmatCommand *cmd, GmatCommand *prev)
//------------------------------------------------------------------------------
/**
 * Inserts GmatCommand into the GmatCommand sequence
 * 
 * @param cmd  Pointer to GmatCommand that is inserted
 * @param prev Pointer to GmatCommand preceding this GmatCommand
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool GmatCommand::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   #ifdef DEBUG_COMMAND_INSERT
   ShowCommand("GmatCommand::", "Insert() this = ", this, " next = ", next);
   ShowCommand("GmatCommand::", "Insert() cmd = ", cmd, " prev = ", prev);
   #endif
   
   if (this == prev)
   {
      GmatCommand *temp = next;
      
      if (!next)
         return Append(cmd);
      
      #ifdef DEBUG_COMMAND_APPEND
      ShowCommand("GmatCommand::", " Setting next of ", this, " to ", cmd);
      #endif
      
      next = cmd;
      
      #if DEBUG_COMMAND_INSERT
      ShowCommand("GmatCommand::", " Setting previous of ", cmd, " to ", prev);
      #endif
      
      cmd->previous = prev;
      
      return next->Append(temp); // assumes cmd->next is NULL and next != NULL
   }
   
   if (next == NULL)
      return false;
   
   return next->Insert(cmd, prev);
}


//------------------------------------------------------------------------------
//  GmatCommand* Remove(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Removes GmatCommand from the GmatCommand sequence
 * 
 * @param cmd Pointer to GmatCommand that is inserted
 * 
 * @return Pointer to the removed GmatCommand or NULL if the GmatCommand was not
 *         found.
 */
//------------------------------------------------------------------------------
GmatCommand* GmatCommand::Remove(GmatCommand *cmd)
{
   #ifdef DEBUG_COMMAND_REMOVE
   ShowCommand("GmatCommand::", "Remove() this = ", this, " cmd = ", cmd);
   ShowCommand("GmatCommand::", "Remove() next = ", next, " prev = ", previous);
   #endif
   
   if (this->IsOfType("BranchEnd"))
   {
      return NULL;
   }
   
   if (this == cmd)
   {  // NULL the next pointer
      next = NULL;
      return this;
   }
   
   if (next == NULL)
      return NULL;
   
   if (next == cmd)
   {
      GmatCommand *temp = next;
      
      #ifdef DEBUG_COMMAND_REMOVE
      ShowCommand("GmatCommand::", " Setting next of ", this, " to ", next->GetNext());
      #endif
      
      next = next->GetNext();
      
      // Set previous command
      if (next != NULL)
      {
         #ifdef DEBUG_COMMAND_REMOVE
         ShowCommand("GmatCommand::", " Setting previous of ", next, " to ", this);
         #endif
         
         next->previous = this;
      }
      
      temp->Remove(cmd);
      
      return temp;
   }
   
   return next->Remove(cmd);
}


//------------------------------------------------------------------------------
//  GmatCommand* GetChildCommand(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Returns nested commands.
 *
 * This method returns the entry point for the commands in a branch off of the
 * main command sequence.  The base implementation here always returns NULL; 
 * see the BranchCommand implementation for a less trivial example.
 *
 * @param whichOne Determines which child command entry point is returned.  
 * 
 * @return The pointer to the first GmatCommand in the branch.
 */
//------------------------------------------------------------------------------
GmatCommand* GmatCommand::GetChildCommand(Integer whichOne)
{
   return NULL;
}


//------------------------------------------------------------------------------
//  Integer GetVariableCount()
//------------------------------------------------------------------------------
/**
 * Counts up variables.
 * 
 * @return The number of variables in this command and its children.
 */
//------------------------------------------------------------------------------
Integer GmatCommand::GetVariableCount()
{
   return 0;
}


//------------------------------------------------------------------------------
//  Integer GetGoalCount()
//------------------------------------------------------------------------------
/**
 * Counts up goals.
 * 
 * @return The number of goals in this command and its children.
 */
//------------------------------------------------------------------------------
Integer GmatCommand::GetGoalCount()
{
   return 0;
}


//------------------------------------------------------------------------------
//  bool AssignObjects()
//------------------------------------------------------------------------------
/**
 * Sets the internal object pointers prior to a run.
 *
 * This default implementation doesn't do anything.  
 *
 * Derived classes should override this method to set the internal data pointers 
 * needed to execute the GmatCommand by accessing the various maps of objects
 * and setting corresponding internal pointers.
 * 
 * @return true if the pointers are assigned "correctly", false otherwise.
 */
//------------------------------------------------------------------------------
bool GmatCommand::AssignObjects()
{
   return true;
}


//------------------------------------------------------------------------------
//  bool ClearObjects()
//------------------------------------------------------------------------------
/**
 * Clears the internal object pointers.
 *
 * This default implementation doesn't do anything.  
 *
 * Derived classes should override this method to "uninitialize" internal 
 * pointers when the assignment operator is called on this GmatCommand.
 * 
 * @return true if the pointers are assigned "correctly", false otherwise.
 */
//------------------------------------------------------------------------------
bool GmatCommand::ClearObjects()
{
   return true;
}


//------------------------------------------------------------------------------
//  bool GmatCommand::InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * This method is used to parse Mission Control Sequence commands in the command
 * itself.  Commands that handle internal parsing override the method to
 * implement parsing.  If the method is not overridden, parsing is handled
 * externally, typically in an Interpreter.
 *
 * @return false when parsing is not part of the command, true if parsing is
 *    handled internally.  Parsing errors result in thrown exceptions.
 */
//------------------------------------------------------------------------------
bool GmatCommand::InterpretAction()
{
   return false;
}


//------------------------------------------------------------------------------
//  Integer DepthIncrement()
//------------------------------------------------------------------------------
/**
 * Indicates the change in branch depth for subsequent commands.
 *
 * This default implementation always returns false because the base class does
 * not perform any actions.
 *
 * @return 0 if the depth stays the same, 1 if the depth increases, or -1 if
 *           it decreases 
 */
//------------------------------------------------------------------------------
Integer GmatCommand::DepthIncrement()
{
   return depthChange;
}


//------------------------------------------------------------------------------
//  bool HasPropStateChanged()
//------------------------------------------------------------------------------
/**
 * Indicates that the propagation data was changed by the command.
 *
 * Some commands can change propagation data in a manner that requires that the
 * propagators and force models need to rebuild their data structures, or that 
 * multistep integrators need to perform a restart before proceeding.  The
 * Sandbox manages the communication to perform propagator restarts.  It checks
 * for state changes of this type by calling this method, and then using the 
 * reference object methods to determine which object (or objects) is affected 
 * by the change.
 *
 * @return true if propagation state has changed, false if there was no change.
 */
//------------------------------------------------------------------------------
bool GmatCommand::HasPropStateChanged()
{
   return commandChangedState;
}


//------------------------------------------------------------------------------
//  bool SkipInterrupt()
//------------------------------------------------------------------------------
/**
 * Allows select commands to avoid polling for user interrupts.
 * 
 * @return true if the command can skip polling; false if polling is needed.
 */
bool GmatCommand::SkipInterrupt()
{
   return false;
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * Tells the sequence that the run was ended, possibly before reaching the end.
 *
 * Some commands -- for instance, the BranchCommands -- track state during a 
 * run.  This method is called when the sequence is terminated by the Moderator
 * to reset the commands to an idle state.
 */
//------------------------------------------------------------------------------
void GmatCommand::RunComplete()
{
   #if DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("GmatCommand::RunComplete for (%p)%s\n", this, typeName.c_str());
   #endif
   
   // Reset stream ID and initialized flag
   streamID = -1;
   isInitialized = false;
   
   if (this->IsOfType("BranchEnd"))
      return;
   
   // By default, just call RunComplete on the next command.
   if (next)
   {
      #if DEBUG_RUN_COMPLETE
      MessageInterface::ShowMessage("   Next cmd is a <%p><%s>'%s'\n", next,
                                    (next->GetTypeName()).c_str(), next->GetGeneratingString().c_str());
      if (next->IsOfType("BranchEnd"))
         MessageInterface::ShowMessage
            ("   .. and that cmd is a branchEnd!!!!!!!!! %s\n",
             (next->GetTypeName()).c_str());
      #endif
      
      // Branch command ends point back to start; this line prevents looping
      if (!next->IsOfType("BranchEnd"))
         next->RunComplete();
   }
}


//------------------------------------------------------------------------------
// void SetRunState(Gmat::RunState newState)
//------------------------------------------------------------------------------
/**
 * Toggle for the run state
 *
 * This method is used to set the current execution state for commands.  Some
 * commands that need to know the state in order to determine how to manage
 * internal processes.  For example, the PropagationEnabledComamnds use this
 * setting to manage event location in a solver loop.
 *
 * @param newState The new run state
 */
//------------------------------------------------------------------------------
void GmatCommand::SetRunState(Gmat::RunState newState)
{
   currentRunState = newState;
}


//------------------------------------------------------------------------------
//  void BuildCommandSummary(bool commandCompleted)
//------------------------------------------------------------------------------
/**
 * Stores the data used for the summary string for the command
 *
 * Inherited commands override this method for specialized summary data.
 * 
 * @param commandCompleted True if the command ran successfully.
 */
//------------------------------------------------------------------------------
void GmatCommand::BuildCommandSummary(bool commandCompleted)
{
   #if DEBUG_BUILD_CMD_SUMMARY
   MessageInterface::ShowMessage
      ("GmatCommand::BuildCommandSummary() %s, commandCompleted=%d, "
       "objectMap=%p, globalObjectMap=%p\n    epochData=%p, stateData=%p, parmData=%p, "
       "fuelMassData=%p, satVector.size()=%d\n", GetTypeName().c_str(), commandCompleted, objectMap,
       globalObjectMap, epochData, stateData, parmData, fuelMassData, satVector.size());
   #endif
   
   if (epochData == NULL)
   {
      satsInMaps = 0;
      if (objectMap != NULL)
      {
         // Build summary data for each spacecraft in the object list
         GmatBase *obj;
         for (std::map<std::string, GmatBase *>::iterator i =
               objectMap->begin(); i != objectMap->end(); ++i)
         {
            #ifdef DEBUG_COMMAND_SUMMARY_LIST
               MessageInterface::ShowMessage("Examining %s\n", i->first.c_str());
            #endif
            obj = i->second;

            if (obj == NULL)
               continue;
            
            if (obj->GetTypeName() == "Spacecraft")
            {
               satVector.push_back((SpaceObject*)obj);
               if (satEpochID == -1)
               {
                  satEpochID = obj->GetParameterID("A1Epoch");
                  satCdID = obj->GetParameterID("Cd");
                  satDragAreaID = obj->GetParameterID("DragArea");
                  satCrID = obj->GetParameterID("Cr");
                  satSRPAreaID = obj->GetParameterID("SRPArea");
                  satTankID = obj->GetParameterID("Tanks");
                  satThrusterID = obj->GetParameterID("Thrusters");
                  satDryMassID = obj->GetParameterID("DryMass");
                  satTotalMassID = obj->GetParameterID("TotalMass");
               }

               ++satsInMaps;
            }
         }
      }
      if (globalObjectMap != NULL)
      {
         // Add summary data for each spacecraft in the global object list
         GmatBase *obj;
         for (std::map<std::string, GmatBase *>::iterator i =
               globalObjectMap->begin(); i != globalObjectMap->end(); ++i)
         {
            #ifdef DEBUG_COMMAND_SUMMARY_LIST
               MessageInterface::ShowMessage("Examining %s\n", i->first.c_str());
            #endif
            obj = i->second;

            if (obj == NULL)
               continue;
            
            if (obj->GetTypeName() == "Spacecraft")
            {
               satVector.push_back((SpaceObject*)obj);
               if (satEpochID == -1)
               {
                  satEpochID = obj->GetParameterID("A1Epoch");
                  satCdID = obj->GetParameterID("Cd");
                  satDragAreaID = obj->GetParameterID("DragArea");
                  satCrID = obj->GetParameterID("Cr");
                  satSRPAreaID = obj->GetParameterID("SRPArea");
                  satTankID = obj->GetParameterID("Tanks");
                  satThrusterID = obj->GetParameterID("Thrusters");
                  satDryMassID = obj->GetParameterID("DryMass");
                  satTotalMassID = obj->GetParameterID("TotalMass");
               }

               ++satsInMaps;
            }
         }
      }
      if (satsInMaps > 0)
      {
         if (epochData != NULL)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (epochData, "epochData", this->GetTypeName() + "::BuildCommandSummary()",
                "deleting epochData");
            #endif
            delete [] epochData;
         }
         if (stateData != NULL)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (stateData, "stateData", this->GetTypeName() + "::BuildCommandSummary()",
                "deleting stateData");
            #endif
            delete [] stateData;
         }
         if (parmData != NULL)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (parmData, "parmData", this->GetTypeName() + "::BuildCommandSummary()",
                "deleting parmData");
            #endif
            delete [] parmData;
         }
         if (fuelMassData != NULL)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (fuelMassData, "fuelMassData", this->GetTypeName() + "::BuildCommandSummary()",
                "deleting fuelMassData");
            #endif
            delete [] fuelMassData;
         }

         tankNames.clear();

         epochData = new Real[satsInMaps];
         stateData = new Real[6*satsInMaps];
         parmData  = new Real[7*satsInMaps];

         fuelMassData = new Real[MAX_NUM_TANKS*satsInMaps];
         tankNames.resize(MAX_NUM_TANKS * satsInMaps, "");

         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (epochData, "epochData", this->GetTypeName() + "::BuildCommandSummary()",
             "epochData = new Real[satsInMaps]");
         MemoryTracker::Instance()->Add
            (stateData, "stateData", this->GetTypeName() + "::BuildCommandSummary()",
             "stateData = new Real[satsInMaps]");
         MemoryTracker::Instance()->Add
            (parmData, "parmData", this->GetTypeName() + "::BuildCommandSummary()",
             "parmData = new Real[satsInMaps]");
         MemoryTracker::Instance()->Add
            (fuelMassData, "fuelMassData", this->GetTypeName() + "::BuildCommandSummary()",
             "fuelMassData = new Real[satsInMaps]");
         #endif
      }
   }
   
   #if DEBUG_BUILD_CMD_SUMMARY
   MessageInterface::ShowMessage
      ("GmatCommand::BuildCommandSummary() Now fill in data...\n   "
       "satsInMaps=%d, satVector.size()=%d\n", satsInMaps,
       satVector.size());
   #endif
   
   Integer i6, i7;
   for (Integer i = 0; i < satsInMaps; ++i)
   {
      // Save epoch and state data
      i6 = i * 6;
      epochData[i]      = satVector[i]->GetRealParameter(satEpochID);
      memcpy(&stateData[i6], satVector[i]->GetState().GetState(),
            6*sizeof(Real));

      // Save parameter and fuel tank data
      i7 = i * 7;
      StringArray tanks = satVector[i]->GetStringArrayParameter(satTankID);
      parmData[i7]      = satVector[i]->GetRealParameter(satCdID);
      parmData[i7+1]    = satVector[i]->GetRealParameter(satDragAreaID);
      parmData[i7+2]    = satVector[i]->GetRealParameter(satCrID);
      parmData[i7+3]    = satVector[i]->GetRealParameter(satSRPAreaID);
      parmData[i7+4]    = satVector[i]->GetRealParameter(satDryMassID);
      parmData[i7+5]    = satVector[i]->GetRealParameter(satTotalMassID);
      parmData[i7+6]    = (Real) tanks.size();

      for (Integer ii = 0; ii < parmData[i7+6]; ii++)
      {
         tankNames.at(MAX_NUM_TANKS*i + ii) = tanks.at(ii);
         fuelMassData[MAX_NUM_TANKS*i + ii] = satVector[i]->GetRefObject(Gmat::HARDWARE, tanks.at(ii))->GetRealParameter("FuelMass");
      }
   }
}


//------------------------------------------------------------------------------
//  void BuildCommandSummaryString(bool commandCompleted)
//------------------------------------------------------------------------------
/**
 * Generates the summary string for the command from the buffered data.
 *
 * Inherited commands override this method for specialized summary data.
 *
 * @param commandCompleted True if the command ran successfully.
 */
//------------------------------------------------------------------------------
void GmatCommand::BuildCommandSummaryString(bool commandCompleted)
{
   #ifdef DEBUG_COMMAND_SUMMARY_TYPE
      MessageInterface::ShowMessage(
            "   Now entering BuildCommandSummaryString with commandCompleted = %s, summaryForEntireMission = %s, missionPhysicsBasedOnly = %s, for command %s of type %s which is %s a physics-based command.\n",
            (commandCompleted? "true" : "false"),(summaryForEntireMission? "true" : "false"), (missionPhysicsBasedOnly? "true" : "false"), summaryName.c_str(), typeName.c_str(),
            (physicsBasedCommand? "DEFINITELY" :"NOT"));
   #endif
   std::stringstream data;

   /// if we're writing the entire mission summary for only physics-based commands, return if not a physics-based command
   if (summaryForEntireMission && ((missionPhysicsBasedOnly && (!physicsBasedCommand)) || !includeInSummary))
   {
      data << "";
      commandSummary = data.str();
      return;
   }
   if (!summaryForEntireMission && (!includeInSummary))
   {
      data << "No summary data for " << typeName << " command " << summaryName << "\n";
      commandSummary = data.str();
      return;
   }

   // Write the separator and the name and type of the command first
   if (summaryForEntireMission)
      data << "======  ";
   else
   {
      data << MISSION_CHANGE_MESSAGE;
      data << "        ";
   }

   // Handle special case of EndScript -> we want ScriptEvent for individual command summary
   if ((!summaryForEntireMission) && (typeName == "EndScript"))
      data << "ScriptEvent Command: " << summaryName << "\n";
   // Handle special case of GMAT -> we want Equation for individual command summary
   else if ((!summaryForEntireMission) && (typeName == "GMAT"))
      data << "Equation Command: " << summaryName << "\n";
   else
      data << typeName << " Command: " << summaryName << "\n";

   if (((objectMap == NULL) && (globalObjectMap == NULL)) ||
       (satVector.size() == 0))
   {
         data << "        Command summary is not supported for Propagate (in Single Step Mode)\n";
         data << "        or when the command did not execute due to control logic statements.\n";
         data << "        Please see the next valid command.\n\n";
   }
   else
   {
      if (!commandCompleted)
         data << "Execute the mission to generate command summary data\n";
      else
      {
        // data << "-------------------------------------------"
        //      << "-------------------------------------------\n";
   
         GmatBase   *obj;
         Rvector6   rawState;
         Rvector6   cartState, cartStateInternal;
         Rvector6   kepState;
         Rvector6   modKepState;
         Rvector6   sphStateAZFPA;
         Rvector6   sphStateRADEC;
         Real       ma, ea, ha, meanMotion, orbitEnergy, c3, semilatusRectum, angularMomentum;
         Real       betaAngle, periAltitude, velPeriapsis, velApoapsis, orbitPeriod, lst, mha;
         Real       latitude, longitude, altitude, bDotT = 0.0, bDotR = 0.0, bVectorAngle = 0.0, bVectorMag = 0.0;
         Real       dla = 0.0, rla = 0.0;
         bool       isEccentric, isHyperbolic, originIsCelestialBody;
         SpacePoint *objOrigin       = NULL;
         SpacePoint *cmdOrigin       = NULL;
         Real       originEqRad      = 0.0;
         Real       originMu         = 0.0;
         Real       originFlattening = 0.0;
         Real       originHourAngle  = 0.0;

         CoordinateConverter cc;
         A1Mjd       a1;

         // Build summary data for each spacecraft in the object list
         for (Integer i = 0; i < satsInMaps; ++i)
         {
            obj                      = satVector[i];
            a1                       = epochData[i];

            objOrigin                = ((SpaceObject*)obj)->GetOrigin(); // is this the same as the origin of the internalCoordSystem?
            summaryCoordSys          = (CoordinateSystem*) FindObject(summaryCoordSysName);
            if (summaryCoordSys == NULL)
            {
               std::string errmsg = "Cannot use coordinate system ";
               errmsg += summaryCoordSysName + " to build command summary.\n";
               errmsg += "Please rerun the mission to make " + summaryCoordSysName + " available.\n";
               throw CommandException(errmsg);
            }
            cmdOrigin                = summaryCoordSys->GetOrigin();
            if (!cmdOrigin)
               throw CommandException("Origin for summary coordinate system is NULL!!!!!\n");


            if (cmdOrigin->IsOfType("CelestialBody")) // This is required to be true, for now
            {
               originIsCelestialBody    = true;
               originEqRad              = ((CelestialBody*)cmdOrigin)->GetEquatorialRadius();
               originMu                 = ((CelestialBody*)cmdOrigin)->GetGravitationalConstant();
               originFlattening         = ((CelestialBody*)cmdOrigin)->GetFlattening();
               originHourAngle          = ((CelestialBody*)cmdOrigin)->GetHourAngle(a1);
            }
            else
            {
               originIsCelestialBody    = false;
            }


            rawState                 = &stateData[i*6];  // assumes in internalCoordSystem
            cartStateInternal        = rawState;
            // First convert the cartesian state to the summaryCoordSys
            #ifdef DEBUG_COMMAND_SUMMARY_STATE
               MessageInterface::ShowMessage("Now converting from %s to %s coordinate system.\n",
                     internalCoordSys->GetName().c_str(), summaryCoordSys->GetName().c_str());
               MessageInterface::ShowMessage("epoch = %12.10f\n", a1.Get());
               MessageInterface::ShowMessage("cart state from array    = %s\n", cartStateInternal.ToString().c_str());
               MessageInterface::ShowMessage("cart state in summary CS = %s\n", cartState.ToString().c_str());
            #endif
            cc.Convert(a1, cartStateInternal, internalCoordSys, cartState, summaryCoordSys);
            // Need to convert state to all representations
            kepState        = StateConversionUtil::CartesianToKeplerian(originMu, cartState);
            modKepState     = StateConversionUtil::Convert(cartState, "Cartesian", "ModifiedKeplerian",
                              originMu, originFlattening, originEqRad);
            sphStateAZFPA   = StateConversionUtil::Convert(cartState, "Cartesian", "SphericalAZFPA",
                              originMu, originFlattening, originEqRad);
            sphStateRADEC   = StateConversionUtil::Convert(cartState, "Cartesian", "SphericalRADEC",
                              originMu, originFlattening, originEqRad);

            #ifdef DEBUG_COMMAND_SUMMARY_STATE
               MessageInterface::ShowMessage("keplerian state in summary CS = %s\n", kepState.ToString().c_str());
            #endif

            isEccentric      = false;
            isHyperbolic     = false;
            ea               = 0.0;
            ha               = 0.0;
            if (kepState[1] < (1.0 - GmatOrbitConstants::KEP_ECC_TOL))
            {
               ea                   = StateConversionUtil::TrueToEccentricAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
                                      kepState[1], true) * GmatMathConstants::DEG_PER_RAD;
               isEccentric          = true;
            }
            else if (kepState[1] > (1.0 + GmatOrbitConstants::KEP_TOL)) // *** or KEP_ECC_TOL or need new tolerance for this?  1.0e-10
            {
               ha                   = StateConversionUtil::TrueToHyperbolicAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
                                      kepState[1], true) * GmatMathConstants::DEG_PER_RAD;
               isHyperbolic         = true;
            }
            ma               = StateConversionUtil::TrueToMeanAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
                               kepState[1], !isHyperbolic) * GmatMathConstants::DEG_PER_RAD;


            // Compute the origin-to-sun unit vector
            Rvector3 originToSun(0.0,0.0,0.0);
            if (cmdOrigin->GetName() != SolarSystem::SUN_NAME)
            {
               Rvector3 sunPos      = (solarSys->GetBody(SolarSystem::SUN_NAME))->GetMJ2000Position(a1);
               Rvector3 originPos   = cmdOrigin->GetMJ2000Position(a1);
               originToSun = sunPos - originPos;
               originToSun.Normalize();
            }
            #ifdef DEBUG_COMMAND_SUMMARY_REF_DATA
               MessageInterface::ShowMessage("----> Spacecraft Origin is %s of type %s\n",
                     objOrigin->GetName().c_str(), objOrigin->GetTypeName().c_str());
               MessageInterface::ShowMessage("----> Command Origin for %s is %s of type %s\n",
                     summaryName.c_str(), cmdOrigin->GetName().c_str(), cmdOrigin->GetTypeName().c_str());
            #endif

            Rvector6 relativeState = cartState;
            if (cmdOrigin->GetName() != SolarSystem::EARTH_NAME)
            {
               Rvector6 originState = cmdOrigin->GetMJ2000State(a1);
               relativeState       -= originState;
            }

            meanMotion       = GmatCalcUtil::CalculateKeplerianData("MeanMotion", cartState, originMu);
            semilatusRectum  = GmatCalcUtil::CalculateAngularData("SemilatusRectum", relativeState, originMu, originToSun);
            angularMomentum  = GmatCalcUtil::CalculateAngularData("HMag", cartState, originMu, originToSun);
            betaAngle        = GmatCalcUtil::CalculateAngularData("BetaAngle", cartState, originMu, originToSun);
            orbitEnergy      = GmatCalcUtil::CalculateKeplerianData("Energy", cartState, originMu);
            c3               = GmatCalcUtil::CalculateKeplerianData("C3Energy", cartState, originMu);
            velPeriapsis     = GmatCalcUtil::CalculateKeplerianData("VelPeriapsis", cartState, originMu);
            periAltitude     = modKepState[0] - originEqRad;

            if (isEccentric)
            {
               orbitPeriod       = GmatCalcUtil::CalculateKeplerianData("OrbitPeriod", cartState, originMu);
               velApoapsis       = GmatCalcUtil::CalculateKeplerianData("VelApoapsis", cartState, originMu);
            }
            else  // these shouldn't be written anyway
            {
               orbitPeriod       = 0.0;
               velApoapsis       = 0.0;
            }

            if (originIsCelestialBody)
            {
               #ifdef DEBUG_COMMAND_SUMMARY_REF_DATA
                  MessageInterface::ShowMessage("----> Now creating BodyFixed cs and passing into PlanetData objects.\n");
               #endif
               // Need a origin-centered BodyFixed coordinate system here
                  CoordinateSystem *originBF = CoordinateSystem::CreateLocalCoordinateSystem("OriginBodyFixed", "BodyFixed",
                                               cmdOrigin, NULL, NULL, ((SpacePoint*)obj)->GetJ2000Body(), solarSys);
               if (!originBF)
               {
                  std::string errmsg = "Error creating BodyFixed Coordinate System for origin ";
                  errmsg += objOrigin->GetName() + "\n";
                  throw CommandException(errmsg);
               }
               // Convert to the origin-centered BodyFixed Coordinate System
               Rvector6 cartBodyFixed;
               cc.Convert(a1, cartStateInternal, internalCoordSys, cartBodyFixed, originBF);


               lst                = GmatCalcUtil::CalculatePlanetData("LST", cartBodyFixed, originEqRad, originFlattening, originHourAngle);
               mha                = GmatCalcUtil::CalculatePlanetData("MHA", cartBodyFixed, originEqRad, originFlattening, originHourAngle);
               latitude           = GmatCalcUtil::CalculatePlanetData("Latitude", cartBodyFixed, originEqRad, originFlattening, originHourAngle);
               longitude          = GmatCalcUtil::CalculatePlanetData("Longitude", cartBodyFixed, originEqRad, originFlattening, originHourAngle);
               altitude           = GmatCalcUtil::CalculatePlanetData("Altitude", cartBodyFixed, originEqRad, originFlattening, originHourAngle);
               if (isHyperbolic)
               {
                  try
                  {
                     bDotT            = GmatCalcUtil::CalculateBPlaneData("BDotT", cartState, originMu);
                     bDotR            = GmatCalcUtil::CalculateBPlaneData("BDotR", cartState, originMu);
                     bVectorAngle     = GmatCalcUtil::CalculateBPlaneData("BVectorAngle", cartState, originMu);
                     bVectorMag       = GmatCalcUtil::CalculateBPlaneData("BVectorMag", cartState, originMu);
                     dla              = GmatCalcUtil::CalculateAngularData("DLA", cartState, originMu, originToSun);
                     rla              = GmatCalcUtil::CalculateAngularData("RLA", cartState, originMu, originToSun);
                  }
                  catch (BaseException& be)
                  {
                     MessageInterface::ShowMessage("ERROR!! %s\n", be.GetFullMessage().c_str());
                  }
               }
               else  // these shouldn't be written anyway
               {
                  bDotT            = 0.0;
                  bDotR            = 0.0;
                  bVectorAngle     = 0.0;
                  bVectorMag       = 0.0;
                  dla              = 0.0;
                  rla              = 0.0;
               }
            } // if originIsCelestialBody
            else  // these shouldn't be written anyway
            {
               lst                = 0.0;
               mha                = 0.0;
               latitude           = 0.0;
               longitude          = 0.0;
               altitude           = 0.0;
            }

            //TimeConverterUtil::Convert
            Real utcModJulEpoch     = TimeConverterUtil::Convert(epochData[i],
                                      TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD,
                                      GmatTimeConstants::JD_JAN_5_1941);
            Real taiModJulEpoch     = TimeConverterUtil::Convert(epochData[i],
                                      TimeConverterUtil::A1MJD, TimeConverterUtil::TAIMJD,
                                      GmatTimeConstants::JD_JAN_5_1941);
            Real ttModJulEpoch      = TimeConverterUtil::Convert(epochData[i],
                                      TimeConverterUtil::A1MJD, TimeConverterUtil::TTMJD,
                                      GmatTimeConstants::JD_JAN_5_1941);
            Real tdbModJulEpoch     = TimeConverterUtil::Convert(epochData[i],
                                      TimeConverterUtil::A1MJD, TimeConverterUtil::TDBMJD,
                                      GmatTimeConstants::JD_JAN_5_1941);
            std::string utcString   = TimeConverterUtil::ConvertMjdToGregorian(utcModJulEpoch);
            std::string taiString   = TimeConverterUtil::ConvertMjdToGregorian(taiModJulEpoch);
            std::string ttString    = TimeConverterUtil::ConvertMjdToGregorian(ttModJulEpoch);
            std::string tdbString   = TimeConverterUtil::ConvertMjdToGregorian(tdbModJulEpoch);
            data.flags(std::ios::left);
            data.precision(10);
            data.setf(std::ios::fixed,std::ios::floatfield);
            data.fill('0');
            data.width(20);

            // Add a between-spacecraft break
            if (i > 0)
               data << "\n   ================================================="
                     "=======================\n\n";

            //  Write the epoch data
            data << "        Spacecraft       : " << obj->GetName() << "\n"
                  << "        Coordinate System: " << summaryCoordSysName <<  "\n\n"

                 << "        Time System   Gregorian                     "
                 << "Modified Julian  \n"
                 << "        --------------------------------------------"
                 << "--------------------------    \n"
                 << "        UTC Epoch:    " << utcString << "      " << utcModJulEpoch <<"\n"
                 << "        TAI Epoch:    " << taiString << "      " << taiModJulEpoch <<"\n"
                 << "        TT  Epoch:    " << ttString  << "      " << ttModJulEpoch  <<"\n"
                 << "        TDB Epoch:    " << tdbString << "      " << tdbModJulEpoch <<"\n\n";

            data.unsetf(std::ios::fixed);
            data.unsetf(std::ios::floatfield);
            data.precision(15);
            data.width(20);

            data << "        Cartesian State                       "
                 << "Keplerian State\n"
                                      << "        ---------------------------           "
                                      << "-------------------------------- \n"
                 << "        X  = "   << BuildNumber(cartState[0])  << " km     "
                 << "        SMA  = " << BuildNumber(kepState[0])   << " km\n"
                 << "        Y  = "   << BuildNumber(cartState[1])  << " km     "
                 << "        ECC  = " << BuildNumber(kepState[1])   << "\n"
                 << "        Z  = "   << BuildNumber(cartState[2])  << " km     "
                 << "        INC  = " << BuildNumber(kepState[2])   << " deg\n"
                 << "        VX = "   << BuildNumber(cartState[3])  << " km/sec "
                 << "        RAAN = " << BuildNumber(kepState[3])   << " deg\n"
                 << "        VY = "   << BuildNumber(cartState[4])  << " km/sec "
                 << "        AOP  = " << BuildNumber(kepState[4])   << " deg\n"
                 << "        VZ = "   << BuildNumber(cartState[5])  << " km/sec "
                 << "        TA   = " << BuildNumber(kepState[5])   << " deg\n"
                 << "                                      "        << "        MA   = " << BuildNumber(ma) << " deg\n";
            if (isEccentric)
               data << "                                      "     << "        EA   = " << BuildNumber(ea) << " deg\n";
            else if (isHyperbolic)
               data << "                                      "     << "        HA   = " << BuildNumber(ha) << " deg\n";
            data << "\n        Spherical State                       "
                 << "Other Orbit Data\n"
                 << "        ---------------------------           "
                 << "--------------------------------\n"
                 << "        RMAG = "               << BuildNumber(sphStateAZFPA[0])           << " km   "
                 << "        Mean Motion        = " << BuildNumber(meanMotion, true)           << " deg/sec\n"
                 << "        RA   = "               << BuildNumber(sphStateAZFPA[1])           << " deg  "
                 << "        Orbit Energy       = " << BuildNumber(orbitEnergy, false)         << " km^2/s^2\n"
                 << "        DEC  = "               << BuildNumber(sphStateAZFPA[2])           << " deg  "
                 << "        C3                 = " << BuildNumber(c3, false)                  << " km^2/s^2\n"
                 << "        VMAG = "               << BuildNumber(sphStateAZFPA[3])           << " km/s "
                 << "        Semilatus Rectum   = " << BuildNumber(semilatusRectum, false)     << " km   \n"
                 << "        AZI  = "               << BuildNumber(sphStateAZFPA[4])           << " deg  "
                 << "        Angular Momentum   = " << BuildNumber(angularMomentum, false)     << " km^2/s\n"
                 << "        VFPA = "               << BuildNumber(sphStateAZFPA[5])           << " deg  "
                 << "        Beta Angle         = " << BuildNumber(betaAngle, false)           << " deg  \n"
                 << "        RAV  = "               << BuildNumber(sphStateRADEC[4])           << " deg  "
                 << "        Periapsis Altitude = " << BuildNumber(periAltitude, false)        << " km   \n"
                 << "        DECV = "               << BuildNumber(sphStateRADEC[5])           << " deg  "
                 << "        VelPeriapsis       = " << BuildNumber(velPeriapsis, false)        << " km/s\n";
            if (isEccentric)
            {
               data << "                                       " << "       VelApoapsis        = " << BuildNumber(velApoapsis, false) << " km/s \n"
                    << "                                       " << "       Orbit Period       = " << BuildNumber(orbitPeriod, false) << " s    \n";
            }
            // add planetodetic parameters, if the origin is a Celestial Body
            if (originIsCelestialBody)
            {
               // and include hyperbolic parameters, if appropriate
               if (isHyperbolic)
               {
                  data << "\n        Planetodetic Properties               "
                       << "Hyperbolic Parameters\n"
                       << "        ---------------------------           "
                       << "--------------------------------\n"
                       << "        LST       = "      << BuildNumber(lst, false)              << " deg  "
                       << "   BdotT          = "      << BuildNumber(bDotT, false)            << " km   \n"
                       << "        MHA       = "      << BuildNumber(mha, false)              << " deg  "
                       << "   BdotR          = "      << BuildNumber(bDotR, false)            << " km   \n"
                       << "        Latitude  = "      << BuildNumber(latitude, false)         << " deg  "
                       << "   B Vector Angle = "      << BuildNumber(bVectorAngle, false)     << " deg  \n"
                       << "        Longitude = "      << BuildNumber(longitude, false)        << " deg  "
                       << "   B Vector Mag   = "      << BuildNumber(bVectorMag, false)       << " km   \n"
                       << "        Altitude  = "      << BuildNumber(altitude, false)         << " km   "
                       << "   DLA            = "      << BuildNumber(dla, false)              << " deg  \n"
                       << "                                           "
                       << "   RLA            = "      << BuildNumber(rla, false)              << " deg  \n";
               }
               else
               {
                  data << "\n        Planetodetic Properties \n"
                       << "        ---------------------------\n"
                       << "        LST       = " << BuildNumber(lst, false)           << " deg\n"
                       << "        MHA       = " << BuildNumber(mha, false)           << " deg\n"
                       << "        Latitude  = " << BuildNumber(latitude, false)      << " deg\n"
                       << "        Longitude = " << BuildNumber(longitude, false)     << " deg\n"
                       << "        Altitude  = " << BuildNumber(altitude, false)      << " km\n";
               }
            }

            data << "\n\n        Spacecraft Properties \n"
                 << "        ------------------------------\n"
                 << "        Cd                    = " << BuildNumber(parmData[i*7],   false, 10) << "\n"
                 << "        Drag area             = " << BuildNumber(parmData[i*7+1], false, 10) << " m^2\n"
                 << "        Cr                    = " << BuildNumber(parmData[i*7+2], false, 10) << "\n"
                 << "        Reflective (SRP) area = " << BuildNumber(parmData[i*7+3], false, 10) << " m^2\n";

            data << "        Dry mass              = " << BuildNumber(parmData[i*7+4])            << " kg\n";
            data << "        Total mass            = " << BuildNumber(parmData[i*7+5])            << " kg\n";

            Integer numTanks = (Integer) parmData[i*7+6];
            if (numTanks > 0)  data << "\n        Tank masses:\n";

            for (Integer kk = 0; kk < numTanks; kk++)
            {
               Integer nameSize = (tankNames.at(kk)).length();
               data << "           " << tankNames.at(MAX_NUM_TANKS*i + kk) << ": ";
               for (Integer mm = 0; mm < 19-nameSize; mm++)  data << " " ;
               data << BuildNumber(fuelMassData[MAX_NUM_TANKS*i+kk]) << " kg\n";
            }
           data << "\n";
         }    // for i 0 -> satsInMaps
      }
   }

   commandSummary = data.str();
}


//------------------------------------------------------------------------------
//  void BuildMissionSummaryString()
//------------------------------------------------------------------------------
/**
 * Generates the summary string for a Mission by building the summary string for 
 * this command, and then tacking on the summary for subsequent commands.
 *
 * Inherited commands can override this method to handle branching or other 
 * special cases.
 * 
 * @param head The first command in the summary
 */
//------------------------------------------------------------------------------
const std::string GmatCommand::BuildMissionSummaryString(const GmatCommand* head)
{
   #ifdef DEBUG_MISSION_SUMMARY
      MessageInterface::ShowMessage("Entering BuildMissionSummaryString for command %s of type %s\n",
            ((GmatCommand*)head)->GetSummaryName().c_str(), ((GmatCommand*)head)->GetTypeName().c_str());
   #endif
   BuildCommandSummaryString();
   std::string missionSummary = commandSummary;


   if (next && (next != head))
   {
      next->SetupSummary(summaryCoordSysName, true, missionPhysicsBasedOnly);
      #ifdef DEBUG_MISSION_SUMMARY
         MessageInterface::ShowMessage("... about to ask for summary for command %s of type %s\n",
               next->GetSummaryName().c_str(), next->GetTypeName().c_str());
      #endif
      missionSummary += next->BuildMissionSummaryString(next);
   }
   return missionSummary;
}


//------------------------------------------------------------------------------
// const std::string BuildNumber(Real value,  bool useExp = false,
//       Integer length)
//------------------------------------------------------------------------------
/**
 * Builds a formatted string containing a Real, so the Real can be serialized to
 * the display
 *
 * @param value  The Real that needs to be serialized
 * @param useExp Use scientific notation
 * @param length The size of the desired string
 *
 * @return The formatted string
 */
//------------------------------------------------------------------------------
const std::string GmatCommand::BuildNumber(Real value, bool useExp,
      Integer length)
{
   std::string retval = "Invalid number";

   if (length < 100)
   {
      char temp[100], defstr[40];
      Integer fraction = 1;

      // check for a NaN first
      if ((!GmatMathUtil::IsEqual(value, 0.0)) &&
         (GmatMathUtil::IsEqual(value, GmatRealConstants::REAL_UNDEFINED)        ||
          GmatMathUtil::IsEqual(value, GmatRealConstants::REAL_UNDEFINED_LARGE)  ||
          GmatMathUtil::IsNaN(value)))
      {
         sprintf(defstr, "%%%ds", length);
         sprintf(temp, defstr, "NaN");
      }
      else
      {
         Real shift = GmatMathUtil::Abs(value);
         if (useExp || (shift > GmatMathUtil::Exp10((Real)length-3)))
         {
            fraction = length - 8;
            sprintf(defstr, "%%%d.%de", length, fraction);
         }
         else
         {
            while (shift > 10.0)
            {
               ++fraction;
               shift *= 0.1;
            }
            fraction = length - 3 - fraction;
            sprintf(defstr, "%%%d.%dlf", length, fraction);
         }
         #ifdef DEBUG_DEFSTR
            MessageInterface::ShowMessage("defstr = %s\n", defstr);
            if (fraction < 0) MessageInterface::ShowMessage("   and fraction = %d\n", fraction);
         #endif
         sprintf(temp, defstr, value);
      }
      retval = temp;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool HasConfigurationChanged()
//------------------------------------------------------------------------------
bool GmatCommand::HasConfigurationChanged()
{
   return commandChanged;
}


//------------------------------------------------------------------------------
// void ConfigurationChanged(bool tf)
//------------------------------------------------------------------------------
void GmatCommand::ConfigurationChanged(bool tf)
{
   #ifdef DEBUG_CONFIG_CHANGED
   MessageInterface::ShowMessage
      ("GmatCommand::ConfigurationChanged() entered, tf = %d\n", tf);
   #endif
   
   commandChanged = tf;
}

//------------------------------------------------------------------------------
// bool HasAFunction()
//------------------------------------------------------------------------------
bool GmatCommand::HasAFunction()
{
   // this default implementation just returns false
   return false;
}


//------------------------------------------------------------------------------
// bool NeedsServerStartup()
//------------------------------------------------------------------------------
/**
 * Indicates in the engine needs to start an external process to run the command
 * 
 * The Sandbox calls this method and tells the Moderator to start the MATLAB
 * engine if a true value is returned.  Note that the method name is not MATLAB 
 * specific; future builds may provide additional interfaces so that specific
 * servers can be started -- for example, Octave -- rather than just assuming 
 * that MATLAB is the engine to start.
 * 
 * @return true if a server needs to start, false otherwise.
 */
//------------------------------------------------------------------------------
bool GmatCommand::NeedsServerStartup()
{
   return false;
}

//------------------------------------------------------------------------------
// bool IsExecuting()
//------------------------------------------------------------------------------
/**
 * Indicates whether the command is executing or not.
 *
 * @return true if command is executing
 */
//------------------------------------------------------------------------------
bool GmatCommand::IsExecuting()
{
   return false;
}


//------------------------------------------------------------------------------
// Integer GetCloneCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of clones the object created, so the Assignment command
 * can change their attributes
 *
 * @return The clone count
 */
//------------------------------------------------------------------------------
Integer GmatCommand::GetCloneCount()
{
   return cloneCount;
}


//------------------------------------------------------------------------------
// Gmat* GetClone(Integer cloneIndex = 0)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a clone
 *
 * @param cloneIndex The index of the clone used by objects that have more than
 *                   one clone
 *
 * @return The pointer
 */
//------------------------------------------------------------------------------
GmatBase* GmatCommand::GetClone(Integer cloneIndex)
{
   return NULL;
}


//------------------------------------------------------------------------------
// bool AffectsClones()
//------------------------------------------------------------------------------
/**
 * Identifies commands that update objects that could be cloned in other objects
 *
 * @return true if a potential clone update is possible, false (the default) if
 *         not
 */
//------------------------------------------------------------------------------
bool GmatCommand::AffectsClones()
{
   return false;
}


//------------------------------------------------------------------------------
// GmatBase* GetUpdatedObject()
//------------------------------------------------------------------------------
/**
 * Retrieves object that has updates so it can be passed to owned clones
 *
 * @return The object pointer, or NULL (the default) if there is no such object
 */
//------------------------------------------------------------------------------
GmatBase* GmatCommand::GetUpdatedObject()
{
   return NULL;
}


//------------------------------------------------------------------------------
// Integer     GetUpdatedObjectParameterIndex()
//------------------------------------------------------------------------------
/**
 * Returns updated parameter index for an updated object that may have clones
 *
 * Owned clone updates might change an entire object, or they might just change
 * a single object parameter.  If the change is to a single parameter, this
 * method returned the parameter index for the changed parameter so that that
 * field, and only that field, can be updated, minimizing disruptions to the
 * underlying class infrastructure.
 *
 * @return The parameter index, or -1 if the change is for the whole object.
 */
//------------------------------------------------------------------------------
Integer GmatCommand::GetUpdatedObjectParameterIndex()
{
   return -1;
}


//------------------------------------------------------------------------------
// virtual void InsertCommandName(std::string &genString)
//------------------------------------------------------------------------------
/**
 * Inserts command name in quotes to generating string. It puts single quotes
 * around the instanceName and inserts right after command type. If instanceName
 * is blank or command type is not found, it does nothing.
 *
 * @param  genString  Input/Output generating string to insert command name
 */
//------------------------------------------------------------------------------
void GmatCommand::InsertCommandName(std::string &genString)
{
   if (instanceName != "")
   {
      // Most commands use typeName as the script syntax, but a few do not --
      // CallMatlabFunction, for example.  So we do a bit of name substitution
      // here
      std::string useName = typeName;
      if ((useName == "CallGmatFunction") || (useName == "CallMatlabFunction"))
      {
         if (GmatGlobal::Instance()->IsWritingGmatKeyword())
            useName = "GMAT";
         else
            useName = "";
      }

      std::string::size_type insertPoint = genString.find_first_of(useName);
      std::string nameInQuotes = "'" + instanceName + "'";
      #if DEBUG_GEN_STRING
      MessageInterface::ShowMessage
         ("   typeName = <%s>, useName = <%s>, command name = <%s>, "
          "insertPoint = %d\n", typeName.c_str(), useName.c_str(),
          instanceName.c_str(), insertPoint);
      #endif
      if (insertPoint != genString.npos)
      {
         insertPoint = insertPoint + useName.size();
         #if DEBUG_GEN_STRING
         MessageInterface::ShowMessage("   insertPoint = %d\n", insertPoint);
         #endif
         genString =
            genString.insert(insertPoint, " " + nameInQuotes);
      }
   }
}


//------------------------------------------------------------------------------
// void ShowCommand(const std::string &prefix = "",
//                  const std::string &title1, GmatCommand *cmd1,
//                  const std::string &title2 = "", GmatCommand *cmd2 = NULL)
//------------------------------------------------------------------------------
/**
 * <static method>
 * Shows command info in the message window.
 *
 * @param prefix A string to write immediately before writing the data,
 *          typically used for indentation
 * @param title1 The substring written for the command in the data
 * @param title2 The substring written for a second command if present
 * @param cmd2 A second command which is also written if provided
 */
//------------------------------------------------------------------------------
void GmatCommand::ShowCommand(const std::string &prefix,
                              const std::string &title1, GmatCommand *cmd1,
                              const std::string &title2, GmatCommand *cmd2)
{
   if (title2 == "")
   {
      MessageInterface::ShowMessage
         ("%s%s: %s<%p><%s>[%s]\n", prefix.c_str(), this->GetTypeName().c_str(),
          title1.c_str(), cmd1, cmd1 ? cmd1->GetTypeName().c_str() : "NULL",
          cmd1 ? cmd1->GetGeneratingString(Gmat::NO_COMMENTS).c_str() : "NULL");
   }
   else
   {
      MessageInterface::ShowMessage
         ("%s%s:\n   %s<%p><%s>[%s]\n   %s<%p><%s>[%s]\n", prefix.c_str(),
          this->GetTypeName().c_str(), title1.c_str(), cmd1,
          cmd1 ? cmd1->GetTypeName().c_str() : "NULL",
          cmd1 ? cmd1->GetGeneratingString(Gmat::NO_COMMENTS).c_str() : "NULL",
          title2.c_str(), cmd2, cmd2 ? cmd2->GetTypeName().c_str() : "NULL",
          cmd2 ? cmd2->GetGeneratingString(Gmat::NO_COMMENTS).c_str() : "NULL");
   }
}


//------------------------------------------------------------------------------
// virtual void ShowWrapper(const std::string &prefix, const std::string &title,
//                          ElementWrapper *wrapper)
//------------------------------------------------------------------------------
void GmatCommand::ShowWrapper(const std::string &prefix, const std::string &title,
                              ElementWrapper *wrapper)
{
   MessageInterface::ShowMessage
      ("%s%s wrapper=<%p>, type=%2d, desc='%s'\n", prefix.c_str(), title.c_str(),
       wrapper, wrapper ? wrapper->GetWrapperType() : -1,
       wrapper ? wrapper->GetDescription().c_str() : "NULL");
}


//------------------------------------------------------------------------------
// void ShowObjectMaps(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Writes the object maps to the message window, for use when debugging
 *
 * @param str A string used to identify the message
 */
//------------------------------------------------------------------------------
void GmatCommand::ShowObjectMaps(const std::string &str)
{
   MessageInterface::ShowMessage("%s\n========================================"
         "==============================\n",
       str.c_str());
   MessageInterface::ShowMessage
      ("GmatCommand::ShowObjectMaps() objectMap=<%p>, globalObjectMap=<%p>\n",
       objectMap, globalObjectMap);
   
   if (objectMap)
   {
      MessageInterface::ShowMessage
         ("Here is the local object map for %s:\n", this->GetTypeName().c_str());
      for (std::map<std::string, GmatBase *>::iterator i = objectMap->begin();
           i != objectMap->end(); ++i)
         MessageInterface::ShowMessage
            ("   %30s  <%p> [%s]\n", i->first.c_str(), i->second,
             i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   }
   if (globalObjectMap)
   {
      MessageInterface::ShowMessage
         ("Here is the global object map for %s:\n", this->GetTypeName().c_str());
      for (std::map<std::string, GmatBase *>::iterator i = globalObjectMap->begin();
           i != globalObjectMap->end(); ++i)
         MessageInterface::ShowMessage
            ("   %30s  <%p> [%s]\n", i->first.c_str(), i->second,
             i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   }
   MessageInterface::ShowMessage("============================================"
         "==========================\n");
}


//------------------------------------------------------------------------------
// StringArray InterpretPreface()
//------------------------------------------------------------------------------
/**
 * Method used to do the common InterpretAction startup stuff
 * 
 * @return The top level chunks from the instruction contained in the generatng 
 *         string.
 */
//------------------------------------------------------------------------------
StringArray GmatCommand::InterpretPreface()
{
   #ifdef DEBUG_INTERPRET_PREFACE
      MessageInterface::ShowMessage("In GmatCommand::InterpretPreface, "
            "generatingString = %s\n", generatingString.c_str());
   #endif
   parser.EvaluateBlock(generatingString);
   StringArray blocks = parser.DecomposeBlock(generatingString);
   StringArray chunks = parser.ChunkLine();
   
   #ifdef DEBUG_INTERPRET_PREFACE
      MessageInterface::ShowMessage("   after call to parser, "
            "generatingString = %s\n", generatingString.c_str());
      MessageInterface::ShowMessage("   and blocks are:\n");
      for (unsigned int ii = 0; ii < blocks.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", (blocks.at(ii)).c_str());
      MessageInterface::ShowMessage("   and chunks are:\n");
      for (unsigned int ii = 0; ii < chunks.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", (chunks.at(ii)).c_str());
   #endif
   
   // First comes the command keyword
   if (chunks[0] != typeName && typeName != "GMAT")
      throw CommandException(
         "Line \"" + generatingString +
         "\"\n should be a " + typeName + " command, but the \"" + typeName +
         "\" keyword is not the opening token in the line.\n");
   
   return chunks;
}

//------------------------------------------------------------------------------
// bool IsSettable(const std::string &setDesc)
//------------------------------------------------------------------------------
/**
 * Method used to check a string and see if it is local data
 * 
 * @param setDesc The string being checked.
 * 
 * @return true if the string is associated with a local data member, false if
 *         it is not.
 */
//------------------------------------------------------------------------------
bool GmatCommand::IsSettable(const std::string &setDesc)
{
   if (find(settables.begin(), settables.end(), setDesc) != settables.end())
      return true;
   return false;

}

//------------------------------------------------------------------------------
// bool SeparateEquals(const std::string &description, std::string &lhs,
//                     std::string &rhs, bool checkOp)
//------------------------------------------------------------------------------
/**
 * Method used to separate 'lhs = rhs' style strings into a lhs and rhs.
 * 
 * @param description The string that needs to be separated.
 * @param lhs         The resulting left hand side.
 * @param rhs         The resulting right hand side, or the empty string if
 *                      there was no right side.
 * @param checkOp     Flag indicating whether or not to check to make sure
 *                      there are no "compound" operators using equals signs
 *                      (e.g. "<=", "==", ">=")
 * 
 * @return true if the string was separated into two pieces, false if only one
 *         piece.  The method throws if more than 2 pieces were found.
 */
//------------------------------------------------------------------------------
bool GmatCommand::SeparateEquals(const std::string &description,
                                 std::string &lhs, std::string &rhs,
                                 bool checkOp)
{
   if (checkOp)
      if ( (description.find("==",0) != description.npos) ||
           (description.find(">=",0) != description.npos) ||
           (description.find("<=",0) != description.npos) ||
           (description.find("=>",0) != description.npos) ||
           (description.find("=<",0) != description.npos) ||
           (description.find("~=",0) != description.npos) ||
           (description.find("=~",0) != description.npos) )
      {
         std::string msg = "The string \"" + description;
         msg += "\" contains a disallowed relational operator for this command: expecting \"=\" ";
         throw CommandException(msg);
      }
   StringArray sides = parser.SeparateBy(description, "= ");
   #ifdef DEBUG_SEPARATE
      MessageInterface::ShowMessage("In SeparateEquals, description = %s\n",
      description.c_str());
      MessageInterface::ShowMessage("----  and sides = \n");
      for (Integer ii = 0; ii < (Integer) sides.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", sides[ii].c_str());    
   #endif
   lhs = sides[0];

   if (sides.size() < 2)
      return false;

   if (sides.size() == 2)
      rhs = sides[1];
   else
      rhs = "";

   if (sides.size() > 2)
   {
      std::string msg = "Error decomposing the string \"";
      msg += description;
      msg += "\"\nTrying to separate into lhs and rhs on \"=\" sign, but found ";
      msg += "too many pieces or missing separator character(s).\n";
      throw CommandException(msg);
   }

   return true;
}


//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Locates an object by name from the object stores or solar system
 *
 * @param name The object's name
 *
 * @return A pointer to the object
 */
//------------------------------------------------------------------------------
GmatBase* GmatCommand::FindObject(const std::string &name)
{
   std::string newName = name;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find('(');
   if (index != name.npos)
      newName = name.substr(0, index);
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("GmatCommand::FindObject() entered, name='%s', newName='%s'\n", name.c_str(),
       newName.c_str());
   #endif
   
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps();
   #endif
   
   // Check for SolarSystem (loj: 2008.06.25)
   if (name == "SolarSystem")
   {
      if (solarSys)
         return solarSys;
      else
         return NULL;
   }
   
   // Check for the object in the Local Object Store (LOS) first
   if (objectMap && objectMap->find(newName) != objectMap->end())
   {
      #ifdef DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage
         ("GmatCommand::FindObject() '%s' found in LOS, so returning <%p>\n",
          newName.c_str(), (*objectMap)[newName]);
      #endif
      return (*objectMap)[newName];
   }
   
   // If not found in the LOS, check the Global Object Store (GOS)
   if (globalObjectMap && globalObjectMap->find(newName) != globalObjectMap->end())
   {
      #ifdef DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage
         ("GmatCommand::FindObject() '%s' found in GOS, so returning <%p>\n",
          newName.c_str(), (*globalObjectMap)[newName]);
      #endif
      return (*globalObjectMap)[newName];
   }
   
   // Let's try SolarSystem (loj: 2008.06.04)
   if (solarSys && solarSys->GetBody(newName))
   {
      #ifdef DEBUG_FIND_OBJECT
      MessageInterface::ShowMessage
         ("GmatCommand::FindObject() '%s' found in SolarSystem, so returning <%p>\n",
          newName.c_str(), (GmatBase*)(solarSys->GetBody(newName)));
      #endif
      return (GmatBase*)(solarSys->GetBody(newName));
   }
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("GmatCommand::FindObject() '%s' not found, so returning NULL\n",
       newName.c_str());
   #endif
   
   return NULL;
}


//------------------------------------------------------------------------------
// bool GmatCommand::SetWrapperReferences(ElementWrapper &wrapper)
//------------------------------------------------------------------------------
/**
 * Sets up reference objects for wrappers
 *
 * @param wrapper The wrapper that is being configured
 *
 * @return true on success, false if an object is not found; throws on error
 */
//------------------------------------------------------------------------------
bool GmatCommand::SetWrapperReferences(ElementWrapper &wrapper)
{
   if (&wrapper != NULL)
   {
      #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage
            ("GmatCommand::SetWrapperReferences() Setting refs for wrapper \"%s\"\n", 
            wrapper.GetDescription().c_str());
      #endif
      StringArray onames = wrapper.GetRefObjectNames();
      
      for (StringArray::const_iterator j = onames.begin(); j != onames.end(); ++j)
      {
         std::string name = *j;
         #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage("      name='%s', now finding object...\n", name.c_str());
         #endif
         GmatBase *obj = FindObject(name);
         if (obj == NULL)
         {
            if (name == "")
               continue;
            
            #ifdef DEBUG_WRAPPER_CODE
            MessageInterface::ShowMessage
               ("GmatCommand::SetWrapperReferences() throwing an exception, could not find object '%s'\n",
                name.c_str());
            #endif
            
            throw CommandException(
                  "GmatCommand::SetWrapperReferences failed to find object named \"" + 
                  name + "\" in: \n   \"" + GetGeneratingString(Gmat::NO_COMMENTS) + "\"\n");
         }
         
         #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage
            ("      Found object <%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
         MessageInterface::ShowMessage("      Now setting wrapper reference object to <%p>\n", obj);
         #endif
         
         if (wrapper.SetRefObject(obj) == false)
         {
            MessageInterface::ShowMessage
               ("GmatCommand::SetWrapperReferences failed to set object named \"%s\", "
                "name in the map is \"%s\"\n", name.c_str(), obj->GetName().c_str());
            return false;
         }
         #ifdef DEBUG_WRAPPER_CODE
            MessageInterface::ShowMessage("      Successfully set reference object \"%s\"\n", 
               name.c_str());
         #endif
      }
   }
   else
   {
      #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage
         ("GmatCommand::SetWrapperReferences() throwing an exception, wrapper is NULL\n");
      #endif
      throw CommandException("GmatCommand::SetWrapperReferences was passed a "
         "NULL object instead of a wrapper in:\n   \"" + generatingString + "\"\n");
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("GmatCommand::SetWrapperReferences() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void ClearOldWrappers()
//------------------------------------------------------------------------------
/**
 * Clears the array of old wrappers
 */
//------------------------------------------------------------------------------
void GmatCommand::ClearOldWrappers()
{
   oldWrappers.clear();
}



//------------------------------------------------------------------------------
// void CollectOldWrappers(ElementWrapper **wrapper)
//------------------------------------------------------------------------------
/**
 * Collects wrappers into a collection of old wrappers
 *
 * @param wrapper The wrapper that might be collected
 */
//------------------------------------------------------------------------------
void GmatCommand::CollectOldWrappers(ElementWrapper **wrapper)
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("GmatCommand::CollectOldWrappers() <%p>'%s' entered, wrapper=<%p>\n",
       this, GetTypeName().c_str(), *wrapper);
   MessageInterface::ShowMessage("   There are %d old wrappers\n", oldWrappers.size());
   #endif
   
   if (*wrapper)
   {
      if (find(oldWrappers.begin(), oldWrappers.end(), *wrapper) == oldWrappers.end())
      {
         oldWrappers.push_back(*wrapper);
         *wrapper = NULL;
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("GmatCommand::CollectOldWrappers() <%p>'%s' leaving\n", this, GetTypeName().c_str());
   MessageInterface::ShowMessage("   There are %d old wrappers\n", oldWrappers.size());
   #endif
}


//------------------------------------------------------------------------------
// void DeleteOldWrappers()
//------------------------------------------------------------------------------
/**
 * Deletes the wrappers from the old wrapper list
 */
//------------------------------------------------------------------------------
void GmatCommand::DeleteOldWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("GmatCommand::DeleteOldWrappers() <%p>'%s' entered, has %d old wrappers\n",
       this, GetTypeName().c_str(), oldWrappers.size());
   #endif
   
   ElementWrapper *wrapper;
   WrapperArray wrappersToDelete;
   
   // Add wrappers to delete
   for (UnsignedInt i = 0; i < oldWrappers.size(); ++i)
   {
      wrapper = oldWrappers[i];
      if (find(wrappersToDelete.begin(), wrappersToDelete.end(), wrapper) ==
          wrappersToDelete.end())
      {
         wrappersToDelete.push_back(wrapper);
         #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage("   <%p> added to wrappersToDelete\n", wrapper);
         #endif
      }
   }
   
   // Delete wrappers
   for (UnsignedInt i = 0; i < wrappersToDelete.size(); ++i)
   {
      wrapper = wrappersToDelete[i];
      #ifdef DEBUG_WRAPPER_CODE
      ShowWrapper("   ", "wrapper:", wrapper);
      #endif
      if (wrapper)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (wrapper, wrapper->GetDescription(), "GmatCommand::ClearWrappers()",
             GetTypeName() + " deleting wrapper");
         #endif
         delete wrapper;
         wrapper = NULL;
      }
   }
   
   oldWrappers.clear();
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("GmatCommand::DeleteOldWrappers() <%p>'%s' leaving, has %d old wrappers\n",
       this, GetTypeName().c_str(), oldWrappers.size());
   #endif
}


//------------------------------------------------------------------------------
// void PrepareToPublish(bool publishAll)
//------------------------------------------------------------------------------
/**
 * Prepares the command to interact with the Publisher
 *
 * @param publishAll Flag used to identify what to publish
 */
//------------------------------------------------------------------------------
void GmatCommand::PrepareToPublish(bool publishAll)
{
   StringArray owners, elements;

   if (publishAll)
   {
      owners.push_back("All");
      elements.push_back("All.epoch");
   }

   streamID = publisher->RegisterPublishedData(this, streamID, owners,
         elements);
}


//------------------------------------------------------------------------------
// void GmatCommand::PublishData()
//------------------------------------------------------------------------------
/**
 * Pushes data to the Publisher
 */
//------------------------------------------------------------------------------
void GmatCommand::PublishData()
{
   publisher->Publish(this, streamID, NULL, 0);
}
