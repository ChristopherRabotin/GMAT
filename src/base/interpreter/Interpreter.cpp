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

#include "Interpreter.hpp" // class's header file
#include "Moderator.hpp"
#include "StringTokenizer.hpp"
#include "ConditionalBranch.hpp"
#include "StringUtil.hpp"  // for ToReal()
#include "Array.hpp"
#include "Assignment.hpp"  // for GetLHS(), GetRHS()
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
#include <sstream>         // for std::stringstream
#include <fstream>         // for std::ifstream used in GMAT functions
#include <stack>

#define __ENABLE_ATTITUDE_LIST__
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
//#define DEBUG_ASSEMBLE_FOR
//#define DEBUG_ASSEMBLE_CALL_FUNCTION
//#define DEBUG_SET
//#define DEBUG_SET_FORCE_MODEL
//#define DEBUG_SET_SOLAR_SYS
//#define DEBUG_FINAL_PASS
//#define DEBUG_FINAL_PASS_CS
//#define DEBUG_CHECK_OBJECT
//#define DEBUG_CHECK_BRANCH
//#define DEBUG_SPECIAL_CASE
//#define DEBUG_WRAPPERS
//#define DEBUG_HANDLE_ERROR
//#define DEBUG_PARSE_REPORT

//------------------------------------------------------------------------------
// Interpreter()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Interpreter::Interpreter()
{
   initialized = false;
   continueOnError = true;   
   parsingDelayedBlock = false;
   ignoreError = false;
   
   theModerator  = Moderator::Instance();
   theReadWriter = ScriptReadWriter::Instance();
   
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
// GmatCommand* InterpretGMATFunction(const std::string &pathAndName)
//------------------------------------------------------------------------------
/**
 * Reads a GMATFunction file and builds the corresponding command stream.
 * 
 * @param <pathAndName> The full path and name for the GMAT function file
 *
 * @return The head of the generated command list.
 */
//------------------------------------------------------------------------------
GmatCommand* Interpreter::InterpretGMATFunction(const std::string &pathAndName)
{
   //=============================================
   //loj: 9/27/06 Need rework using the TextParser
   //=============================================

   return NULL;

   /*
   #ifdef DEBUG_FUNCTION_PARSING
      MessageInterface::ShowMessage(
         "Entered Interpreter::InterpretGMATFunction(%s)...\n", 
         pathAndName.c_str());
   #endif

   // Verify that the function file can be found
   std::ifstream funFile;
   funFile.open(pathAndName.c_str());
   if (funFile.is_open() == false)
      throw InterpreterException("Could not open the GMAT function file " + 
         pathAndName);
         
   // Start reading and parsing the file
   std::string currentLine;
   char buffer[4096];
   
   UnsignedInt loc, lineNum = 0;;
   bool functionDefined = false;

   std::string block = "";

   // Set the start of the command stream
   GmatCommand *commands = NULL;
   
   bool commandNotEncountered = true;
   
   // Parse the file
   funFile.getline(buffer, 4094);
   while (!funFile.eof())
   {
      ++lineNum;
      currentLine = buffer;

      #ifdef DEBUG_FUNCTION_PARSING
         MessageInterface::ShowMessage("%d: %s\n", lineNum, 
            currentLine.c_str());
      #endif
      
      // Truncate the line at the comment character, if there is one
      loc = currentLine.find('%', 0);
      if (loc != std::string::npos)
         currentLine = currentLine.substr(0, loc);
      
      // Handle the function definition line
      if (!functionDefined)
      {
         loc = currentLine.find("GmatFunction ", 0);
         if (loc != std::string::npos)
         {
            UnsignedInt funLoc = loc + 13;
            UnsignedInt parenLoc = currentLine.find('(', 0);

            std::string functionName = 
               currentLine.substr(funLoc,parenLoc-funLoc);
            
            if (parenLoc == std::string::npos)
            {
               std::string errstr = 
                  "Attempting to define a GmatFunction in the file '";
               throw InterpreterException(errstr + pathAndName + 
                  "', but the file definition line\n   '" +
                  currentLine + 
                  "'\ndoes not contain an input argument list " +
                  "(missing parentheses?)");
            }

            StringArray invals, outvals;
            std::string subLine, argLine;
            
            argLine = currentLine.substr(parenLoc);
            invals = SeparateParens(argLine);
            
            loc = currentLine.find("=");
            if (loc != std::string::npos)
            {
               subLine = currentLine.substr(0, loc);
            
               Integer start = SkipWhiteSpace(0, subLine);
               if (start >=0)
                  subLine = subLine.substr(start);
               if (subLine[0] == '[')
                  outvals = SeparateBrackets(subLine);
               else
                  outvals.push_back(subLine);
            }
   
            #ifdef DEBUG_FUNCTION_PARSING
               MessageInterface::ShowMessage("Function:  '%s'\n", 
                  functionName.c_str());
               MessageInterface::ShowMessage("Inputs:\n");
               for (StringArray::iterator i = invals.begin(); 
                    i != invals.end(); ++i)
                  MessageInterface::ShowMessage("   %s\n", i->c_str());
               MessageInterface::ShowMessage("Outputs:\n");
               for (StringArray::iterator i = outvals.begin(); 
                    i != outvals.end(); ++i)
                  MessageInterface::ShowMessage("   %s\n", i->c_str());
            #endif

            commands = theModerator->CreateCommand("BeginFunction");
            commands->SetStringParameter("FunctionName", functionName);
            for (StringArray::iterator i = invals.begin(); i != invals.end();
                 ++i)
               commands->SetStringParameter("Input", *i);
            for (StringArray::iterator i = outvals.begin(); i != outvals.end();
                 ++i)
               commands->SetStringParameter("Output", *i);
            functionDefined = true;
         }
      }
      else  // Toss the remaining lines into the buffer for parsing
      {
         // Creation of local objects is handled first
         loc = currentLine.find("Create ", 0);
         if (loc != std::string::npos)
         {
            // Handle Create lines
            std::string type, name;
            StringArray createLine = SeparateSpaces(currentLine);
            if (createLine[0] != "Create")
            {
               std::string errstr = 
                  "Create needs to be the first word in the line \n'";
               throw InterpreterException(errstr + currentLine + "'");
            }
            
            type = createLine[1]; 
            name = createLine[2];
            
            GmatBase *obj; 
            InterpretObject(type, "", &obj);
            if (!obj)
               throw InterpreterException("Unable to create object " +
                                           name + " of type " + type +
                                           "\nFunction text: \"" + 
                                           currentLine + "\"");
            obj->SetName(name);

            //#ifdef DEBUG_FUNCTION_PARSING
               MessageInterface::ShowMessage("Created a %s named '%s'\n", 
                  obj->GetTypeName().c_str(), obj->GetName().c_str());
            //#endif

            if (commands == NULL)
               throw InterpreterException(
                  "Did not create BeginFunction command");
            commands->SetRefObject(obj, Gmat::UNKNOWN_OBJECT, name);
         }
         else
         {
            if (commandNotEncountered)
            {
               StringArray paramLine = SeparateSpaces(currentLine);
               if ((paramLine.size() > 3) && (paramLine[0] == "GMAT"))
               {
                  MessageInterface::ShowMessage("GMAT line:  '%s'\n", 
                     currentLine.c_str());
                  
                  // Format should be "GMAT obj.parm = val;"
                  if (paramLine[2] == "=")
                  {
                     GmatBase *obj = NULL;
                     Integer parmID;
                     std::string value;

                     StringArray objectData = SeparateDots(paramLine[1]);
                     if (objectData.size() >= 2)
                     {
                        obj = commands->GetRefObject(Gmat::UNKNOWN_OBJECT, 
                                 objectData[0]);
                                 
                        parmID = obj->GetParameterID(objectData[1]);                              
                        if (obj != NULL)
                        {
                           MessageInterface::ShowMessage("Setting %s on %s\n",
                              objectData[1].c_str(), obj->GetName().c_str());
                           SetParameter(obj, parmID, paramLine[3]);
                        }
                     }
                  }
               }
               else
               {
                  block += currentLine + "\n";
                  if (paramLine.size() > 0)
                  {
                     if (find(commandList.begin(),commandList.end(),paramLine[0]) != 
                         commandList.end())
                     {
                        MessageInterface::ShowMessage("Command found:  '%s'\n",
                           paramLine[0].c_str()); 
                        commandNotEncountered = false;
                     }
                  }
               }
            }
            else
            {
               // Add all others to the block of commands that are parsed
               block += currentLine + "\n";
            }
         }
      }
      
      funFile.getline(buffer, 4095);
   }
   
   if (commands == NULL)
      throw InterpreterException("Did not create BeginFunction command");
      
   InterpretTextBlock(commands, block);
   
   // Close the file when done, and end the function stream
   funFile.close();
   
   #ifdef DEBUG_FUNCTION_PARSING
      MessageInterface::ShowMessage(
         "...Finished parsing '%s'.\n", 
         pathAndName.c_str());

      MessageInterface::ShowMessage("Here is the generated command stream:\n");
      GmatCommand *cmd = commands;
      while (cmd != NULL)
      {
         std::string cmdtxt = cmd->GetGeneratingString(Gmat::SCRIPTING, "   ");
         MessageInterface::ShowMessage("%s\n", cmdtxt.c_str());
         cmd = cmd->GetNext();
      } 

      throw InterpreterException("A temporary break to check parsing");
   #endif
   
   return commands;
   */
}


//------------------------------------------------------------------------------
// StringArray& GetListOfObjects(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type.
 *
 * @param  type  object type
 *
 * @return array of configured item names; return empty array if none
 */
//------------------------------------------------------------------------------
StringArray& Interpreter::GetListOfObjects(Gmat::ObjectType type)
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
// GmatBase* CreateObject(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to build core objects and put them in the ConfigManager.
 *  
 * @param  type  Type for the requested object.
 * @param  name  Name for the object, used for references to the object.
 * 
 * @return object pointer on success, NULL on failure.
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::CreateObject(const std::string &type,
                                    const std::string &name)
{
   #ifdef DEBUG_CREATE_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::CreateObject() type=%s, name=%s\n",
       type.c_str(), name.c_str());
   #endif
   
   GmatBase *obj = NULL;

   if (!GmatStringUtil::IsValidName(name, true))
   {
      InterpreterException ex
          (type + " object can not be named to \"" + name + "\"");
      HandleError(ex);
      return NULL;
   }
   
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
   
   // This error message may be confusing to users
   // check for name first
   if ((name != "EarthMJ2000Eq") && 
       (name != "EarthMJ2000Ec") && 
       (name != "EarthFixed"))
   {
      obj = FindObject(name);
      // Since Parameters are created automatically as they are referenced,
      // add check for PARAMETER type
      if (obj != NULL && obj->GetType() != Gmat::PARAMETER)
      {
         InterpreterException ex
            (type + " object named \"" + name + "\" already exist");
         HandleError(ex, true, true);
         return obj;
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
      
      #ifdef __ENABLE_ATTITUDE_LIST__
      // Handle Attitude
      else if (find(attitudeList.begin(), attitudeList.end(), type) != 
               attitudeList.end())
         obj = (GmatBase*)theModerator->CreateAttitude(type, name);
      #endif
      
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
      
      // Handle System Parameters
      else if (find(parameterList.begin(), parameterList.end(), type) != 
               parameterList.end())
         obj = (GmatBase*)CreateParameter(type, name);
      
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
         ("Interpreter::CreateObject() type=%s, name=%s successfully created\n",
          obj->GetTypeName().c_str(), obj->GetName().c_str());
   }
   #endif
   
   return obj;
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
   return theModerator->GetSolarSystemInUse();
}

//------------------------------------------------------------------------------
// bool ValidateCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Checks the input command to make sure it wrappers are set up for it
 * correctly, if necessary.
 *
 * @param <cmd> the command to validate
 */
//------------------------------------------------------------------------------
bool Interpreter::ValidateCommand(GmatCommand *cmd)
{
   cmd->ClearWrappers();
   const StringArray wrapperNames = cmd->GetWrapperObjectNameArray();
   #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage("In Validate, wrapper names are:\n");
      for (Integer ii=0; ii < (Integer) wrapperNames.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", wrapperNames[ii].c_str());
   #endif
   
   //---------------------------------------------------------------------------
   // Special case for Assignment command (LHS = RHS).
   // Since such as Sat.X can be both Parameter or ObjectProperty, we want to
   // create a Parameter wapper if RHS is a Parameter for Assignment command.
   // So special code is needed to tell the CreateElementWrapper() to check for
   // Parameter first.
   //---------------------------------------------------------------------------
   if (cmd->GetTypeName() == "GMAT")
   {
      Assignment *acmd = (Assignment*)cmd;
      
      // Handle LHS
      std::string lhs = acmd->GetLHS();
      ElementWrapper *ew = CreateElementWrapper(lhs);
      if (cmd->SetElementWrapper(ew, lhs) == false)
      {
         InterpreterException ex
            ("Undefined object \"" + lhs + "\" found in command \"" +
             cmd->GetTypeName());
         HandleError(ex);
         return false;
      }
      
      // Handle RHS
      // Note: Assignment::GetWrapperObjectNameArray() returns only for RHS elements
      for (StringArray::const_iterator i = wrapperNames.begin();
           i != wrapperNames.end(); ++i)
      {
         std::string name = (*i);
         if (name != "")
         {
            if (IsParameterType(name))
               ew = CreateElementWrapper(name, true);
            else
               ew = CreateElementWrapper(name);
            
            if (cmd->SetElementWrapper(ew, name) == false)
            {
               InterpreterException ex
                  ("Undefined object \"" + name + "\" found in Math Assignment");
               HandleError(ex);
               return false;
            }
         }
      }
      
      // Set math wrappers to math tree
      acmd->SetMathWrappers();
   }
   else
   {
      for (StringArray::const_iterator i = wrapperNames.begin();
           i != wrapperNames.end(); ++i)
      {
         ElementWrapper *ew = CreateElementWrapper(*i);
         
         if (cmd->SetElementWrapper(ew, *i) == false)
         {
            InterpreterException ex
               ("Undefined object \"" + (*i) + "\" found in command \"" +
                cmd->GetTypeName());
            HandleError(ex);
            return false;
         }
      }
   }
   
   return CheckUndefinedReference(cmd);
}


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
      ElementWrapper *ew = CreateElementWrapper(*i, true);
      
      if (sub->SetElementWrapper(ew, *i) == false)
      {
         InterpreterException ex
            ("ElementWrapper for \"" + (*i) +
             "\" cannot be created for the Subscriber \"" + obj->GetName() + "\"");
         HandleError(ex, false);
         return false;
      }
   }
   
   return true;
}


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
      ("Interpreter::FindPropertyID() obj = %p, chunk = <%s>\n", obj,
       chunk.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(chunk);
   Integer count = parts.size();
   std::string prop = parts[count-1];
   
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
      ("Interpreter::FindPropertyID() returning owner=%p, retval=%d\n", owner, retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name, const std::string &ofType = "")
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to find a configured object.
 *
 * @param  name    Name of the object.
 * @param  ofType  Type of object required; leave blank for no checking
 *
 * @return Configured object pointer
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::FindObject(const std::string &name, 
                                  const std::string &ofType)
{
   GmatBase *obj;
   if (name == "SolarSystem")
      obj =  theModerator->GetSolarSystemInUse();
   else
      obj =  theModerator->GetConfiguredObject(name);
   // check for the requested type
   if ((obj != NULL) && (ofType != "") && (!obj->IsOfType(ofType))) obj = NULL;
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
      ("Interpreter::CreateCommand() type='%s', inCmd=%p, \n   desc='%s'\n", type.c_str(),
       inCmd, desc.c_str());
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
      //WriteParts("calling IsObjectType()", parts);
      
      // Check for valid object type first before creating a command,
      // since UnknownCommand Variable var1 still creates CallFunction command
      
      if (IsObjectType(parts[0]))
      {
         InterpreterException ex
            ("Found invalid command \"" + type + "\"");
         HandleError(ex);
      }
      else if (!GmatStringUtil::IsValidName(type + desc))
      {
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
         ("   => %s created.\n", cmd->GetTypeName().c_str());
   else
      MessageInterface::ShowMessage
         ("   => %s created and appended to %s.\n",
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
      ("Interpreter::AssembleCommand() cmd='%s'\n   desc='%s'\n",
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
   MessageInterface::ShowMessage("   Setting funcName=%s\n", funcName.c_str());
   #endif
   
   // Set function name to CallFunction
   if (funcName != "")
   {
      retval = cmd->SetStringParameter("FunctionName", funcName);
   }
   else
   {
      InterpreterException ex("*** ERROR *** Found blank function name in: \n\"" +
                              currentBlock + "\"\n");
      HandleError(ex);
      return false;
   }
   
   #ifdef DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage("   Setting input\n");
   WriteParts("CallFunction Input" , inArray);
   #endif
   
   // Set input to CallFunction
   bool validParam = false;
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
         ("   lhs:%s, op:%s, rhs:%s\n", parts[i].c_str(), parts[i+1].c_str(),
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
         MessageInterface::ShowMessage("   logOp=%s\n", parts[i+3].c_str());
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
      ("Interpreter::AssembleForCommand() desc='%s'\n", desc.c_str());
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
      MessageInterface::ShowMessage("   %s\n", parts[ii].c_str());
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
      ("Interpreter::AssembleForCommand() index=%s, start=<%s>, end=<%s>, "
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
   StringArray parts;
   Integer count;
   //StringArray parts = theTextParser.Decompose(desc, "()");
   //Integer count = parts.size();
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("AssembleGeneralCommand() cmd='%s', desc='%s'\n", cmd->GetTypeName().c_str(),
       desc.c_str());
   WriteParts("AssembleGeneralCommand()", parts);
   #endif
   
   if (type == "Target" || type == "Report" || type == "BeginFiniteBurn" ||
       type == "EndFiniteBurn" || type == "Optimize")
   {
      // first item is ref. object name
      
      if (type == "Target")
      {
         // this command, for compatability with MATLAB, should not have
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
         parts = theTextParser.Decompose(desc, "()");
         count = parts.size();
         cmd->SetRefObjectName(Gmat::SOLVER, parts[0]);
         
         // Make sure there is only one thing on the line
         if (parts.size() > 1)
         {
            InterpreterException ex
               ("Unexpected text at end of Target command");
            HandleError(ex);
            retval = false;
         }
         
         // Check if the Solver exist
         GmatBase *obj = FindObject(parts[0], "Solver");
         if (obj == NULL)
         {
            InterpreterException ex
               ("Cannot find the Solver \"" + parts[0] + "\"");
            HandleError(ex);
            retval = false;
         }
      }
      else if (type == "Optimize")
      {
         // this command, for compatability with MATLAB, should not have
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
         parts = theTextParser.Decompose(desc, "()");
         count = parts.size();
         cmd->SetRefObjectName(Gmat::SOLVER, parts[0]);
         // Make sure there is only one thing on the line
         if (parts.size() > 1)
         {
            InterpreterException ex
               ("Unexpected text at end of Optimize command");
            HandleError(ex);
            retval = false;
         }
         
         // Check if the Solver exist
         GmatBase *obj = FindObject(parts[0], "Solver");
         if (obj == NULL)
         {
            InterpreterException ex
               ("Cannot find the Solver \"" + parts[0] + "\"");
            HandleError(ex);
            retval = false;
         }
         // 2007.09.14 wcs - should only have a Solver name on the line
         //for (int i=1; i<count; i++)
         //   cmd->SetRefObjectName(Gmat::PARAMETER, parts[i]);
      }
      else if (type == "Report")
      {
         // this command, for compatability with MATLAB, should not have
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
         parts = GmatStringUtil::SeparateBy(desc, " ", true);
         //parts = theTextParser.SeparateBrackets(desc, "()", " ,", false);
         Integer count = parts.size();
         
         #ifdef DEBUG_PARSE_REPORT 
            MessageInterface::ShowMessage("Parsing Report, parts are:\n");
            for (Integer ii=0; ii < count; ii++)
               MessageInterface::ShowMessage("   %d:   %s\n", ii, parts.at(ii).c_str());
         #endif
         
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
      else
      {
         #ifdef DEBUG_ASSEMBLE_COMMAND
         MessageInterface::ShowMessage("Begin/EndFiniteBurn being processed ...");
         #endif
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
         parts = theTextParser.Decompose(desc, "()", false);
         
         #ifdef DEBUG_ASSEMBLE_COMMAND
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
            //StringArray subParts = theTextParser.SeparateBrackets(parts[1], "()", " ,");
            StringArray subParts = theTextParser.SeparateBrackets(parts[1], "()", ",");
            
            #ifdef DEBUG_ASSEMBLE_COMMAND
            WriteParts(type, subParts);
            #endif
            
            count = subParts.size();
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
      }
   }
   else if (type == "Save")
   {
      // these commands, for compatability with MATLAB, should not have
      // parentheses (except to indicate array elements), brackets, or braces
      if (!GmatStringUtil::HasNoBrackets(desc, false))
      {
         std::string msg = 
            "The Save command is not allowed to contain brackets, braces, or "
            "parentheses";
         InterpreterException ex(msg);
         HandleError(ex);
         return false;
      }
      // we only want to separate by spaces - commas are not allowed, 
      // not even in arrays (for this command)
      parts = GmatStringUtil::SeparateBy(desc, " ", true);
      unsigned int saveCount = parts.size();
      bool isOk = true;
      for (unsigned int i=0; i<saveCount; i++)
      {
         if (parts[i].find(',') != parts[i].npos)
         {
            std::string msg = 
               "The Save command is not allowed to contain commas - "
               "separate objects by spaces";
            InterpreterException ex(msg);
            HandleError(ex);
            isOk = false;
         }
         else
            cmd->SetRefObjectName(Gmat::UNKNOWN_OBJECT, parts[i]);
      }
      if (!isOk) return false;
      //for (int i=0; i<count; i++)
      //   cmd->SetRefObjectName(Gmat::UNKNOWN_OBJECT, parts[i]);
   }
   else
   {
      retval = false;
   }
   
   #ifdef DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("   AssembleGeneralCommand() leaving setting %s, retval=%d\n",
       type.c_str(), retval);
   #endif
   
   return retval;
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
      ("Interpreter::CreateAssignmentCommand() lhs=%s, rhs=%s\n", lhs.c_str(),
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
// Parameter* CreateSystemParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a system Parameter from the input parameter name. If the name contains
 * dots, it consider it as a system parameter.  If it is not a system Parameter
 * it checks if object by given name is a Parameter.
 *
 * @param name parameter name to be parsed for Parameter creation
 *             Such as, sat1.Earth.ECC, sat1.SMA
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
      param = CreateParameter(paramType, str, ownerName, depName);
      
      #ifdef DEBUG_CREATE_PARAM
      MessageInterface::ShowMessage
         ("   1) parameter created with paramType=%s, ownerName=%s, depName=%s\n",
          paramType.c_str(), ownerName.c_str(), depName.c_str());
      #endif
   }
   else
   {
      // Find the object and check if it is a Parameter
      GmatBase *obj = FindObject(str);
      if (obj != NULL && obj->GetType() == Gmat::PARAMETER)
         param = (Parameter*)obj;
   
      #ifdef DEBUG_CREATE_PARAM
      MessageInterface::ShowMessage
         ("   2) parameter created with str=%s, but obj = %s, and param = %s\n", 
          str.c_str(),
          (obj == NULL? "NULL!!" : "not NULL"),
          (param == NULL? "NULL!!" : "not NULL"));
      #endif
   }
   
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
 * @param  ownerName  object name of parameter requested
 * @param  depName    Dependent object name of parameter requested
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
      ("Interpreter::CreateParameter() type=%s, name=%s, ownerName=%s, depName=%s\n",
       type.c_str(), name.c_str(), ownerName.c_str(), depName.c_str());
   #endif
   
   // Check if create an array
   if (type == "Array")
      return CreateArray(name);
   else
      return theModerator->CreateParameter(type, name, ownerName, depName);
}


//------------------------------------------------------------------------------
// Parameter* CreateArray(onst std::string &arrayStr)
//------------------------------------------------------------------------------
Parameter* Interpreter::CreateArray(const std::string &arrayStr)
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
   
   Parameter *param = theModerator->CreateParameter("Array", name);

   #ifdef DEBUG_CREATE_ARRAY
   MessageInterface::ShowMessage
      ("Interpreter::CreateArray() row=%d, col=%d\n", row, col);
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
      ("Interpreter::GetArrayIndex() arrayStr=%s, name=%s, rowStr=%s, "
       "colStr=%s, row=%d, col=%d\n", arrayStr.c_str(), name.c_str(),
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
      ("Interpreter::MakeAssignment() lhs=%s, rhs=%s\n", lhs.c_str(), rhs.c_str());
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
      ("   isLhsObject=%d, isLhsArray=%d, lhsPropName=%s\n",
       isLhsObject, isLhsArray, lhsPropName.c_str());
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
         if (toType == Gmat::STRING_TYPE)
            toObj->SetStringParameter("Value", rhsParam->GetString());
         else if (toType == Gmat::REAL_TYPE)
            toObj->SetRealParameter("Value", rhsParam->EvaluateReal());
      }
      else
      {
         if (toType == Gmat::STRING_TYPE)
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
      ("Interpreter::SetValueToObject() objType=%s, toObj=%s, value=%s\n",
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
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage
         ("===> calling toObj->SetStringParameter(Value, %s)\n", value.c_str());
      #endif
      
      toObj->SetStringParameter("Value", value);
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
            ("The value of \"" + objName + "\" for field name\"" + toProp +
             "\" on ForceModel \"" + toOwner->GetName() + "\" is not an allowed value");
         HandleError(ex);
         return false;
      }
   }
   else
   {
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
      
      #ifdef DEBUG_SET
      MessageInterface::ShowMessage("   toId=%d, toType=%d\n", toId, toType);
      #endif
      
      if (fromObj->GetTypeName() == "Variable" || fromObj->GetTypeName() == "String")
      {
         Gmat::ParameterType fromType = ((Parameter*)fromObj)->GetReturnType();
         
         if (fromType == toType)
         {
            if (toType == Gmat::STRING_TYPE)
               toObj->SetStringParameter(toId, fromObj->GetStringParameter("Value"));
            else if (toType == Gmat::REAL_TYPE)
               toObj->SetRealParameter(toId, fromObj->GetRealParameter("Value"));
         }
         else if (toObj->GetTypeName() == "Variable" && fromType == Gmat::REAL_TYPE)
         {
            toObj->SetStringParameter(toId, fromObj->GetName());
         }
         else
         {
            if (fromObj->GetTypeName() == "String" &&
                (toType == Gmat::STRING_TYPE || toType == Gmat::STRINGARRAY_TYPE))
            {
               toObj->SetStringParameter(toId, fromObj->GetStringParameter("Value"));
            }
            else if (toType == Gmat::OBJECT_TYPE || toType == Gmat::OBJECTARRAY_TYPE)
            {
               toObj->SetStringParameter(toId, fromObj->GetName());
            }
            else
            {
               //InterpreterException ex
               //   ("Cannot set Variable or String to property. "
               //    "Property types are not the same.");
               InterpreterException ex
                  ("The value of \"" + fromObj->GetName() + "\" for field name\"" + toProp +
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
            ("   Setting objType=%s, objName=%s\n", fromObj->GetTypeName().c_str(),
             fromObj->GetName().c_str());
         #endif
         
         try
         {
            toObj->SetStringParameter(toProp, fromObj->GetName());
            if (toType == Gmat::OBJECT_TYPE)
               toObj->SetRefObject(fromObj, fromObj->GetType(), fromObj->GetName());
         }
         catch (BaseException &e)
         {
            HandleError(e);
            return false;
         }
      }
   }
   
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
         if (toType == Gmat::STRING_TYPE)
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
            (errorMsg1 + "field name \"" + toProp + "\" on object " + "\"" +
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
   
   Integer toId = toOwner->GetParameterID(toProp);
   Gmat::ParameterType toType = toOwner->GetParameterType(toId);
   
   if (toType != Gmat::REAL_TYPE)
   {
      //InterpreterException ex
      //   ("Cannot set Array element to property. Property type is not Real.");
      InterpreterException ex
         ("The value of \"" + fromArray + "\" for field name\"" + toProp +
          "\" on object " + "\"" + toOwner->GetName() + "\" is not an allowed value");
      HandleError(ex);
      return false;
   }
   
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
            (errorMsg1 + "field name \"" + toProp + "\" on object " + "\"" +
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
         Parameter *param = NULL;
         
         // Try create Parameter first if it is not ObjectType
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
         
         if (param != NULL)
         {
            // Other than Subscriber, it can only take STRING_TYPE parameter
            if (param->GetReturnType() == Gmat::STRING_TYPE ||
                obj->IsOfType(Gmat::SUBSCRIBER))
            {
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling SetStringParameter(%d, %s)\n", id, value.c_str());
               #endif
               
               obj->SetStringParameter(id, value);
            }
            else
            {
               errorMsg1 = errorMsg1 + "The value of \"" + value + "\" for ";
               errorMsg2 = "  The allowed value is Object Name";
               break;
            }
         }
         else
         {
            // check if value is not a number
            Real rval;
            Integer ival;
            if (GmatStringUtil::ToReal(value, rval, true) ||
                GmatStringUtil::ToInteger(value, ival, true))
            {
               errorMsg1 = errorMsg1 + "The value of \"" + value + "\" for ";
               errorMsg2 = "  The allowed value is Object Name";
               break;
            }
            
            // Create Owned Object, if it is object type
            GmatBase *ownedObj = NULL;
            if (IsObjectType(value))
               ownedObj = CreateObject(value, "");
            
            #ifdef DEBUG_SET
            if (ownedObj)
               MessageInterface::ShowMessage
                  ("   Created ownedObjType: %s\n",
                   ownedObj->GetTypeName().c_str());
            #endif
            
            if (ownedObj)
            {
               #ifdef DEBUG_SET
               MessageInterface::ShowMessage
                  ("   Calling SetRefObject(%s(%p), %d)\n", ownedObj->GetTypeName().c_str(),
                   ownedObj, ownedObj->GetType());
               #endif
               
               obj->SetRefObject(ownedObj, ownedObj->GetType(), ownedObj->GetName());
            }
            else
            {
               // Should I create Variable?
               if (obj->GetType() == Gmat::SUBSCRIBER)
               {
                  CreateParameter("Variable", value);
                  obj->SetStringParameter(id, value);
               }
               else
               {
                  // Special case of InternalForceModel in script
                  if (value != "InternalForceModel")
                  {
                     // Set as String parameter, so it can be caught in FinalPass()
                     obj->SetStringParameter(id, value);
                     
                     //InterpreterException ex
                     //   ("*** Internal Error ***Cannot create " + value + "\n");
                     //HandleError(ex);
                     //return false;
                  }
               }
            }
         }
         
         retval = true;
         break;
      }
   case Gmat::INTEGER_TYPE:
   case Gmat::UNSIGNED_INT_TYPE:
      {
         Integer ival;
         if (GmatStringUtil::ToInteger(valueToUse, ival))
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling SetIntegerParameter(%d, %d)\n", id, ival);
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
               ("   Calling SetUnsignedIntParameter(%d, %d, %d)\n", id, ival, index);
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
               ("   Calling SetRealParameter(%d, %s)\n", id, rvalStr.c_str());
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
   case Gmat::STRING_TYPE:
      {
         if (index >= 0)
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling SetStringParameter(%d, %s, %d)\n", id,
                valueToUse.c_str(), index);
            #endif
            
            retval = obj->SetStringParameter(id, valueToUse, index);
         }
         else
         {
            #ifdef DEBUG_SET
            MessageInterface::ShowMessage
               ("   Calling SetStringParameter(%d, %s)\n", id, valueToUse.c_str());
            #endif
            
            retval = obj->SetStringParameter(id, valueToUse);
         }
         break;
      }
   case Gmat::STRINGARRAY_TYPE:
      {
         #ifdef DEBUG_SET
         MessageInterface::ShowMessage
            ("   Calling SetStringParameter(%d, %s, %d)\n", id,
             valueToUse.c_str(), index);
         #endif
         
         try
         {
            retval = obj->SetStringParameter(id, valueToUse);
         }
         catch (BaseException &e)
         {
            // try with index
            retval = obj->SetStringParameter(id, valueToUse, index);
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
               ("   Calling SetBooleanParameter(%d, %d)\n", id, tf);
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
            ("   Calling SetOnOffParameter(%d, %s)\n", id, valueToUse.c_str());
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
   else if (type == Gmat::STRING_TYPE)
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
   if (value.find("{") != value.npos || value.find("{") != value.npos)
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
   //GMAT FM.CentralBody          = Earth;
   //GMAT FM.PrimaryBodies        = {Earth};
   //GMAT FM.PointMasses          = {Sun, Luna, Jupiter}; 
   //GMAT FM.Drag                 = None;
   //GMAT FM.Gravity.Earth.Model  = JGM2;
   //GMAT FM.Gravity.Earth.Degree = 20;
   //GMAT FM.Gravity.Earth.Order  = 20;
   //GMAT FM.SRP                  = On;
   
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
// AxisSystem* Interpreter::CreateAxisSystem(std::string type, GmatBase *owner)
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
// void HandleError(BaseException &e, bool writeLine, bool warning)
//------------------------------------------------------------------------------
void Interpreter::HandleError(BaseException &e, bool writeLine, bool warning)
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
// void HandleErrorMessage(BaseException &e, const std::string &lineNumber,
//                         const std::string &line, bool warning)
//------------------------------------------------------------------------------
void Interpreter::HandleErrorMessage(BaseException &e, const std::string &lineNumber,
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
      MessageInterface::ShowMessage("    %s\n", (objList.at(ii)).c_str());
   #endif //------------------------------------------- end debug ----
   
   //----------------------------------------------------------------------
   // Check reference objects
   //----------------------------------------------------------------------
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      obj = GetConfiguredObject(*i);
      
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
               refObj = GetConfiguredObject(owner);
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
      
      //-----------------------------------------------------------------
      // Note: This section needs be modified as needed. 
      // GetRefObjectTypeArray() should be implemented if we want to
      // add to this list. This was added to write specific error messages.
      //-----------------------------------------------------------------
      
      else if (obj->GetType() == Gmat::BURN ||
               obj->GetType() == Gmat::SPACECRAFT ||
               obj->GetType() == Gmat::FORCE_MODEL ||
               obj->GetType() == Gmat::COORDINATE_SYSTEM ||
               obj->GetType() == Gmat::SUBSCRIBER)
      {
         // Set return flag to false if any check failed
         bool retval1 = CheckUndefinedReference(obj, false);
         retval = retval && retval1;
         
         // Subscribers uses ElementWrapper to handle Parameter, Variable,
         // Array, Array elements, so create wrappers in ValidateSubscriber()
         if (retval && obj->GetType() == Gmat::SUBSCRIBER)
            retval = retval && ValidateSubscriber(obj);
      }
      else
      {
         try
         {
            // Check referenced SpacePoint used by configured objects
            refNameList = obj->GetRefObjectNameArray(Gmat::SPACE_POINT);
            
            for (UnsignedInt j = 0; j < refNameList.size(); j++)
            {
               refObj = GetConfiguredObject(refNameList[j]);
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
      CoordinateSystem *cs = (CoordinateSystem*)GetConfiguredObject(*i);
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
         
         refObj = GetConfiguredObject(refNameList[j]);
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
      MessageInterface::ShowMessage("    %s\n", (objList.at(ii)).c_str());
   #endif
   
   for (StringArray::iterator i = objList.begin(); i != objList.end(); ++i)
   {
      obj = GetConfiguredObject(*i);
      if (obj->GetTypeName() == "LibrationPoint")
      {
         std::string primary = obj->GetStringParameter("Primary");
         std::string secondary = obj->GetStringParameter("Secondary");
         //MessageInterface::ShowMessage("===> primary=%s, secondary=%s\n",
         //                              primary.c_str(), secondary.c_str());
         if (primary == secondary)
         {
            InterpreterException ex
               ("The Primay and Secondary bodies cannot be the same in the "
                "LibrationPoint \"" + obj->GetName() + "\"");
            HandleError(ex, false);
            retval = false;
         }
      }
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
      obj = GetConfiguredObject(*i);
      
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      GmatBase *csObj = GetConfiguredObject(csName);
      
      // To catch as many errors we can, continue with next object
      if (csObj == NULL)
         continue;
      
      #ifdef DEBUG_FINAL_PASS
      MessageInterface::ShowMessage
         ("   Calling SetRefObject(%s(%p), %d)\n", csObj->GetName().c_str(),
          csObj, csObj->GetType());
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
   
   if (find(propagatorList.begin(), propagatorList.end(), type) !=
       propagatorList.end())
      return true;
   
   if (find(axisSystemList.begin(), axisSystemList.end(), type) !=
       axisSystemList.end())
      return true;
   
   if (find(atmosphereList.begin(), atmosphereList.end(), type) !=
       atmosphereList.end())
      return true;

   #ifdef __ENABLE_ATTITUDE_LIST__
   if (find(attitudeList.begin(), attitudeList.end(), type) !=
       attitudeList.end())
      return true;
   #endif
   
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
// bool CheckUndefinedReference(GmatBase *obj, bool writeLine)
//------------------------------------------------------------------------------
/*
 * This method checks if reference objects of input object exist. First it
 * gets reference object type list by calling obj->GetRefObjectTypeArray() and
 * then gets reference object name list by calling obj->GetRefObjectNameArray(reftype).
 * If reference object type is Parameter, it checks if owner object of that
 * paramter exist; otherwise, it only check for the reference object name exist.
 *
 * @param  obj  input object of undefined reference object to be checked
 * @param  writeLine  flag indicating whether line number should be written
 *                    for ther error message
 */
//------------------------------------------------------------------------------
bool Interpreter::CheckUndefinedReference(GmatBase *obj, bool writeLine)
{
   bool retval = true;
   ObjectTypeArray refTypes = obj->GetRefObjectTypeArray();
   StringArray refNames;
   
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::CheckUndefinedReference() type='%s', name='%s', refTypes=\n",
       obj->GetTypeName().c_str(), obj->GetName().c_str());
   for (UnsignedInt i=0; i<refTypes.size(); i++)
      MessageInterface::ShowMessage
         ("   %s\n", GmatBase::GetObjectTypeString(refTypes[i]).c_str());
   #endif
   
   // Save command can have any object type, so handle it first
   if (obj->GetTypeName() == "Save")
   {
      refNames = obj->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
      for (UnsignedInt j=0; j<refNames.size(); j++)
      {
         #ifdef DEBUG_CHECK_OBJECT
            MessageInterface::ShowMessage("For Save, name = %s\n", refNames.at(j).c_str()); 
         #endif
         GmatBase *refObj = GetConfiguredObject(refNames[j]);
         if (refObj == NULL)
         {
            InterpreterException ex
               ("Nonexistent object \"" + refNames[j] + "\" referenced in the " +
                obj->GetTypeName() + " command\"");
            HandleError(ex, writeLine);
            retval = false;
         }
         else if (refObj->GetType() == Gmat::PARAMETER)
         {
            // per Steve Hughes 2007.10.16 - arrays are OK - WCS
            // (but not array elements)
            if ( (!(refObj->IsOfType("Array"))) ||
                 (GmatStringUtil::IsParenPartOfArray(refNames[j])) )
            {
               InterpreterException ex
                  ("\"" + refNames[j] + "\" referenced in the " +
                   obj->GetTypeName() + " command is not a saveable object");
               HandleError(ex, writeLine);
               retval = false;
            }
         }
      }
      return retval;
   }
   
   // If Axis is NULL, create default MJ2000Eq
   if (obj->GetType() == Gmat::COORDINATE_SYSTEM)
   {
      if (obj->GetRefObject(Gmat::AXIS_SYSTEM, "") == NULL)
      {
         CreateAxisSystem("MJ2000Eq", obj);
         InterpreterException ex
            ("The CoordinateSystem \"" + obj->GetName() +
             "\" has empty AxisSystem, so default MJ2000Eq was created");
         HandleError(ex, writeLine, true);
      }
   }
   
   // Check if undefined ref. objects exist
   for (UnsignedInt i=0; i<refTypes.size(); i++)
   {
      #ifdef DEBUG_CHECK_OBJECT
      MessageInterface::ShowMessage
         ("   %s\n", GmatBase::GetObjectTypeString(refTypes[i]).c_str());
      #endif
      
      try
      {
         refNames = obj->GetRefObjectNameArray(refTypes[i]);
         
         #ifdef DEBUG_CHECK_OBJECT
         MessageInterface::ShowMessage(" and the refNames are:\n");
         for (UnsignedInt j=0; j<refNames.size(); j++)
            MessageInterface::ShowMessage
               ("      %s\n", refNames[j].c_str());
         #endif
         
         // Check System Parameters seperately since it follows certain naming
         // convention.  "owner.dep.type" where owner can be either Spacecraft
         // or Burn for now
         
         if (refTypes[i] == Gmat::PARAMETER)
         {
            for (UnsignedInt j=0; j<refNames.size(); j++)
            {
               if (GetConfiguredObject(refNames[j]) == NULL)
               {
                  std::string type, ownerName, depObj;
                  GmatStringUtil::ParseParameter(refNames[j], type, ownerName, depObj);
                  
                  // Check only system parameters
                  if (type == "")
                  {
                     InterpreterException ex
                        ("Nonexistent object \"" + refNames[j] + 
                         "\" referenced in the " + obj->GetTypeName() + "\" " +
                         obj->GetName() + "\"");
                     HandleError(ex, writeLine);
                     retval = false;
                  }
                  else
                  {
                     if (GetConfiguredObject(ownerName) == NULL)
                     {
                        InterpreterException ex
                           ("Nonexistent object \"" + ownerName + 
                            "\" referenced in \"" + refNames[j] + "\"");
                        HandleError(ex, writeLine);
                        retval = false;
                     }
                     
                     if (!theModerator->IsParameter(type))
                     {
                        InterpreterException ex
                           ("\"" + type + "\" is not a valid Parameter Type");
                        HandleError(ex, writeLine);
                        retval = false;
                     }
                  }
               }
            }
         }
         else
         {
            for (UnsignedInt j=0; j<refNames.size(); j++)
            {
               std::string objName = obj->GetTypeName();
               if (obj->GetType() != Gmat::COMMAND)
                  objName = objName + " \"" + obj->GetName() + "\"";
               
               GmatBase *refObj = GetConfiguredObject(refNames[j]);
               
               if (refObj == NULL)
               {
                  InterpreterException ex
                     ("Nonexistent " + GmatBase::GetObjectTypeString(refTypes[i]) +
                      " \"" + refNames[j] + "\" referenced in the " + objName);
                  HandleError(ex, writeLine);
                  retval = false;
               }
               else if (!refObj->IsOfType(refTypes[i]))
               {
                  #ifdef DEBUG_CHECK_OBJECT
                  MessageInterface::ShowMessage
                     ("refObj->IsOfType(refTypes[i])=%d, %s\n", refTypes[i],
                      GmatBase::GetObjectTypeString(refTypes[i]).c_str());
                  #endif
                  
                  InterpreterException ex
                     ("\"" + refNames[j] + "\" referenced in the " + objName +
                      " is not an object of " +
                      GmatBase::GetObjectTypeString(refTypes[i]));
                  HandleError(ex, writeLine);
                  retval = false;
               }
            }
         }
      }
      catch (BaseException &e)
      {
         #ifdef DEBUG_CHECK_OBJECT
         // since command handles setting reference object during initialization,
         // skip if object is command
         if (obj->GetType() != Gmat::COMMAND)
            MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
   
   #ifdef DEBUG_CHECK_OBJECT
   MessageInterface::ShowMessage("CheckUndefinedReference() returning %d\n", retval);
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
// ElementWrapper* CreateElementWrapper(const std::string &desc,
//                                      bool parametersFirst = false)
//------------------------------------------------------------------------------
/**
 * Creates the appropriate ElementWrapper, based on the description.
 *
 * @param  <desc>  description string for the element required
 * @param  <parametersFirst>  true if creating for wrappers for the Parameter
 *                            first then Object Property
 *
 * @return pointer to the created ElementWrapper
 */
//------------------------------------------------------------------------------
ElementWrapper* Interpreter::CreateElementWrapper(const std::string &desc,
                                                  bool parametersFirst)
{
   Gmat::WrapperDataType itsType = Gmat::NUMBER;
   ElementWrapper *ew = NULL;
   Real           rval;
   // remove extra parens and blank spaces at either end of string
   std::string    descTrimmed = GmatStringUtil::Trim(desc);
   descTrimmed = GmatStringUtil::RemoveExtraParen(descTrimmed);
   descTrimmed = GmatStringUtil::Trim(descTrimmed);  

   #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage(
         "In Interpreter::CreateElementWrapper, original string is \"%s\"\n",
         desc.c_str(), "\"\n");
      MessageInterface::ShowMessage(
         "                                  and trimmed string is \"%s\"\n",
         descTrimmed.c_str(), "\"\n");
   #endif
   
   // first, check to see if it is a number
   if (GmatStringUtil::ToReal(descTrimmed,rval))
   {
      ew = new NumberWrapper();
      #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage(
            "In Interpreter, created a NumberWrapper for \"%s\"\n",
            descTrimmed.c_str(), "\"\n");
      #endif
   }
   else 
   {
      Parameter *p;
      // check to see if it is an array
      bool isOuterParen;
      Integer openParen, closeParen;
      GmatStringUtil::FindParenMatch(descTrimmed, openParen, closeParen, isOuterParen);
      #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage(
            "In Interpreter, descTrimmed = \"%s\"\n", descTrimmed.c_str());
      #endif
      if ((openParen  != -1) && (closeParen != -1) && (closeParen > openParen))
      {
         std::string arrayName = GmatStringUtil::Trim(descTrimmed.substr(0,openParen));
         #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage(
               "In Interpreter, arrayName = \"%s\"\n", arrayName.c_str());
         #endif
         if (!GmatStringUtil::IsBlank(arrayName))
         {
            if (((p = theModerator->GetParameter(arrayName)) == NULL)  ||
               !(p->IsOfType("Array")) )
            {
               InterpreterException ex("\"" + arrayName + "\"" + " is not an array");
               HandleError(ex);
               return false;
            }
            ew             = new ArrayElementWrapper();
            itsType        = Gmat::ARRAY_ELEMENT;
            #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage(
                  "In Interpreter, created a ArrayElementWrapper for \"%s\"\n",
                  descTrimmed.c_str(), "\"\n");
            #endif
         }
      }
      // check to see if it is an object property or a parameter
      else if (descTrimmed.find(".") != std::string::npos)
      {
         // if parametersFirst, always create parameter wrapper first
         if (parametersFirst)
         {
            Parameter *param = CreateSystemParameter(descTrimmed);
            if (param)
            {
               #ifdef DEBUG_WRAPPERS
                  MessageInterface::ShowMessage(
                     "In Interpreter(1), about to create a ParameterWrapper for \"%s\"\n",
                     descTrimmed.c_str(), "\"\n");
               #endif
               CreateParameterWrapper(param, &ew, itsType);
            }
            else
            {
               InterpreterException ex("\"" + descTrimmed + "\"" + 
                   " is not a valid parameter");
               HandleError(ex);
               return NULL;
            }
         }
         else
         {
            // try to parse the string for an owner and type
            // NOTE - need to check handling of "special cases, e.g.Sat1.X" <<<<<<<<<<
            // check for object parameter first
            std::string type, owner, dep;
            GmatStringUtil::ParseParameter(descTrimmed, type, owner, dep);
            // if it's not a valid object, then see if it's a parameter
            GmatBase *theObj = theModerator->GetConfiguredObject(owner);
            if (theObj == NULL)
            {
               Parameter *param = CreateSystemParameter(descTrimmed);
               
               if (param)
               {
                  #ifdef DEBUG_WRAPPERS
                     MessageInterface::ShowMessage(
                        "In Interpreter(2), about to create a ParameterWrapper for \"%s\"\n",
                        descTrimmed.c_str(), "\"\n");
                  #endif
                  CreateParameterWrapper(param, &ew, itsType);
               }
               else // there is an error
               {
                  ew = new StringWrapper();
                  itsType = Gmat::STRING;
                  
                  #ifdef DEBUG_WRAPPERS
                  MessageInterface::ShowMessage
                     ("In Interpreter, created a StringWrapper for \"%s\"\n",
                              descTrimmed.c_str(), "\"\n");
                  #endif
                  
                  //InterpreterException ex("\"" + descTrimmed + "\"" + 
                  //   " is not a valid object property or parameter");
                  //HandleError(ex);
                  //return NULL;
               }
            }
            else
            {
               // if there are two dots, then treat it as a parameter
               if (descTrimmed.find_first_of(".") != descTrimmed.find_last_of("."))
               {
                  Parameter *param = CreateSystemParameter(descTrimmed);
                  if (param)
                  {
                     #ifdef DEBUG_WRAPPERS
                        MessageInterface::ShowMessage(
                           "In Interpreter(3), about to create a ParameterWrapper for \"%s\"\n",
                           descTrimmed.c_str(), "\"\n");
                     #endif
                     CreateParameterWrapper(param, &ew, itsType);
                  }
                  else // there is an error
                  {
                     InterpreterException ex("\"" + descTrimmed + "\"" + 
                        " is not a valid object property or parameter");
                     HandleError(ex);
                     return NULL;
                  }
               }
               else
               {
                  // if there is no such field for that object, it is a parameter
                  bool isValidProperty = true;
                  try
                  {
                     theObj->GetParameterID(type);
                  }
                  catch (BaseException &be)
                  {
                     isValidProperty = false;
                     Parameter *param = CreateSystemParameter(descTrimmed);
                     if (param)
                     {
                        #ifdef DEBUG_WRAPPERS
                           MessageInterface::ShowMessage(
                              "In Interpreter(4), about to create a ParameterWrapper for \"%s\"\n",
                              descTrimmed.c_str(), "\"\n");
                        #endif
                        CreateParameterWrapper(param, &ew, itsType);
                     }
                     else // there is an error
                     {
                        InterpreterException ex("\"" + descTrimmed + "\"" + 
                           " is not a valid object property or parameter");
                        HandleError(ex);
                        return NULL;
                     }
                  }
                  if (isValidProperty)
                  {
                     ew = new ObjectPropertyWrapper();
                     itsType = Gmat::OBJECT_PROPERTY;
                     #ifdef DEBUG_WRAPPERS
                     MessageInterface::ShowMessage(
                        "In Interpreter, created a ObjectPropertyWrapper for \"%s\"\n",
                        descTrimmed.c_str(), "\"\n");
                     #endif
                  }
               }
            }
         }
      }
      else // check to see if it is a Variable or some other parameter
      {
         p = theModerator->GetParameter(descTrimmed);
         if ( (p) && (p->IsOfType("Variable")) )
         {
            ew = new VariableWrapper();
            itsType = Gmat::VARIABLE;
            #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage(
                     "In Interpreter, created a VariableWrapper for \"%s\"\n",
                     descTrimmed.c_str(), "\"\n");
            #endif
         }
         else if ( (p) && p->IsOfType("Array") )
         {
            ew = new ArrayWrapper();
            itsType = Gmat::ARRAY;
            #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage(
                     "In Interpreter, created an ArrayWrapper for \"%s\"\n",
                     descTrimmed.c_str(), "\"\n");
            #endif
         }
         else if ( (p) && p->IsOfType("String") )
         {
            ew = new StringObjectWrapper();
            itsType = Gmat::STRING_OBJECT;
            #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage(
                     "In Interpreter, created a StringObjectWrapper for \"%s\"\n",
                     descTrimmed.c_str(), "\"\n");
            #endif
         }
         else if ( (p) && p->IsOfType("Parameter") )
         {
            #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage(
                  "In Interpreter(5), about to create a ParameterWrapper for \"%s\"\n",
                  descTrimmed.c_str(), "\"\n");
            #endif
            CreateParameterWrapper(p, &ew, itsType);
         }
         else
         {
            // Check see if it is clonable object
            GmatBase *obj = FindObject(descTrimmed);

            if (obj != NULL)
            {
               ew = new ObjectWrapper();
               itsType = Gmat::OBJECT;
               
               #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage
                  ("In Interpreter, created an ObjectWrapper for \"%s\"\n",
                           descTrimmed.c_str(), "\"\n");
               #endif
            }
            else
            {
               bool bVal;
               if (GmatStringUtil::ToBoolean(descTrimmed, bVal))
               {
                  ew = new BooleanWrapper();
                  itsType = Gmat::BOOLEAN;
                  
                  #ifdef DEBUG_WRAPPERS
                  MessageInterface::ShowMessage
                     ("In Interpreter, created a BooleanWrapper for \"%s\"\n",
                              descTrimmed.c_str(), "\"\n");
                  #endif
               }
               else if (descTrimmed == "On" || descTrimmed == "Off" ||
                        descTrimmed == "on" || descTrimmed == "off")
               {
                  ew = new OnOffWrapper();
                  itsType = Gmat::ON_OFF;
                  
                  #ifdef DEBUG_WRAPPERS
                  MessageInterface::ShowMessage
                     ("In Interpreter, created a OnOffWrapper for \"%s\"\n",
                              descTrimmed.c_str(), "\"\n");
                  #endif
               }
               else
               {
                  ew = new StringWrapper();
                  itsType = Gmat::STRING;
                  
                  #ifdef DEBUG_WRAPPERS
                  MessageInterface::ShowMessage
                     ("In Interpreter, created a StringWrapper for \"%s\"\n",
                              descTrimmed.c_str(), "\"\n");
                  #endif
               }
            }
         }
      }
   }
   
   if (ew)
   {
      // set the description string; this will also setup the wrapper
      ew->SetDescription(desc);
      //ew->SetDescription(descTrimmed); // or do I want this?
      // if it's an ArrayElement, set up the row and column wrappers
      if (itsType == Gmat::ARRAY_ELEMENT)
      {
         std::string    rowName = ((ArrayElementWrapper*)ew)->GetRowName();
         ElementWrapper *row    = CreateElementWrapper(rowName);
         ((ArrayElementWrapper*)ew)->SetRow(row);
         std::string    colName = ((ArrayElementWrapper*)ew)->GetColumnName();
         ElementWrapper *col    = CreateElementWrapper(colName);
         ((ArrayElementWrapper*)ew)->SetColumn(col);
      }
   }
   return ew;
}


//------------------------------------------------------------------------------
// void CreateParameterWrapper(Parameter *param, ElementWrapper **ew,
//                             Gmat::WrapperDataType &itsType)
//------------------------------------------------------------------------------
/*
 * Creates ParameterWrapper.
 *
 * @param <param> Pointer to Paramter
 * @param <ew> Adress of outpout ElementWrapper pointer
 * @param <itsType> outpout wrapper type
 */
//------------------------------------------------------------------------------
void Interpreter::CreateParameterWrapper(Parameter *param, ElementWrapper **ew,
                                         Gmat::WrapperDataType &itsType)
{
   *ew = NULL;
   
   if (param->IsOfType("String"))
   {
      *ew = new StringObjectWrapper();
      itsType = Gmat::STRING_OBJECT;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Interpreter, created a StringObjectWrapper for \"%s\"\n",
          param->GetName().c_str(), "\"\n");
      #endif
   }
   else
   {
      *ew = new ParameterWrapper();
      itsType = Gmat::PARAMETER_OBJECT;
      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("In Interpreter, created a ParameterWrapper for \"%s\"\n",
          param->GetName().c_str(), "\"\n");
      #endif
   }
}

