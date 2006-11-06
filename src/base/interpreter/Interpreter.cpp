//$Header$
//------------------------------------------------------------------------------
//                                  Interpreter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/08/28
// Rework:  2006/09/27
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
#include "StringUtil.hpp"  // for ToDouble()
#include "Array.hpp"
#include "MessageInterface.hpp"
#include <sstream>         // for std::stringstream
#include <fstream>         // for std::ifstream used bt GMAT functions


//#define DEBUG_COMMAND_LIST
//#define DEBUG_OBJECT_LIST
//#define DEBUG_ARRAY_GET 1
//#define DEBUG_CREATE_OBJECT 1
//#define DEBUG_CREATE_PARAM 1
//#define DEBUG_CREATE_ARRAY 1
//#define DEBUG_CREATE_COMMAND 1
//#define DEBUG_ASSIGNMENT 1
//#define DEBUG_ASSEMBLE_COMMAND 1
//#define DEBUG_ASSEMBLE_FOR 1
//#define DEBUG_ASSEMBLE_CALL_FUNCTION 1
//#define DEBUG_SET 1
//#define DEBUG_SET_FORCE_MODEL 1
//#define DEBUG_SET_SOLAR_SYS 1

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
//   continueOnError = false;

   theModerator  = Moderator::Instance();
   theReadWriter = ScriptReadWriter::Instance();
   
   #if DEBUG_INTERP
   MessageInterface::ShowMessage
      ("Interpreter::Interpreter() initialized=%d, theModerator=%p\n",
       initialized, theModerator);
   #endif

   // Not used
//    if (theModerator)
//    {
//       // Set up the mapping for the core types
//       typeMap["Spacecraft"] = Gmat::SPACECRAFT;
//       typeMap["GroundStation"] = Gmat::GROUND_STATION;
//       typeMap["Command"] = Gmat::COMMAND;
//       typeMap["ForceModel"] = Gmat::FORCE_MODEL;
//       typeMap["Force"] = Gmat::PHYSICAL_MODEL;
//       typeMap["SolarSystem"] = Gmat::SOLAR_SYSTEM;
//       typeMap["CelestialBody"] = Gmat::CELESTIAL_BODY;
//       typeMap["Parameter"] = Gmat::PARAMETER;
//       typeMap["StoppingCondition"] = Gmat::STOP_CONDITION;
//       typeMap["Subscriber"] = Gmat::SUBSCRIBER;
//       typeMap["Propagator"] = Gmat::PROP_SETUP;
//       typeMap["Burn"] = Gmat::BURN;
//       typeMap["CoordinateSystem"] = Gmat::COORDINATE_SYSTEM;
//       typeMap["AxisSystem"] = Gmat::AXIS_SYSTEM;
//    }
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
   //MessageInterface::ShowMessage
   //   ("===> Interpreter::Initialize() initialized=%d\n", initialized);

   errorList.clear();
   delayedBlocks.clear();
   inCommandMode = false;
   
   if (initialized)
      return;
   
   // Build a mapping for all of the defined commands
   StringArray cmds = theModerator->GetListOfFactoryItems(Gmat::COMMAND);
   copy(cmds.begin(), cmds.end(), back_inserter(commandList));

   if (cmds.size() == 0)
      return;
   
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
   
   
   #ifdef DEBUG_COMMAND_LIST
      std::vector<std::string>::iterator pos1;
      
      MessageInterface::ShowMessage("\nCommands:\n   ");      
      for (pos1 = cmds.begin(); pos1 != cmds.end(); ++pos1)
         MessageInterface::ShowMessage(*pos1 + "\n   ");
      
   #endif
      
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
// GmatBase* GetObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Interpreter::GetObject(const std::string &name)
{
   return theModerator->GetObject(name);
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
   #if DEBUG_CREATE_OBJECT
   MessageInterface::ShowMessage
      ("Interpreter::CreateObject() type=%s, name=%s\n",
       type.c_str(), name.c_str());
   #endif
   
   GmatBase *obj = NULL;

   // let's check object name
   if (name == "GMAT" || name == "Create")
   {
      InterpreterException ex
          (type + " object can not be named " + name + ".");
          HandleError(ex);
      return NULL;
   }
   
   StringArray commandNames = theModerator->GetListOfFactoryItems(Gmat::COMMAND);
   for (Integer i=0; i<(Integer)commandNames.size(); i++)
   {
      if (commandNames[i] == name)
      {
         InterpreterException ex
            (type + " object can not be named " + name + ".");
            HandleError(ex);
         return NULL;
      }
   }

   // This error message may be confusing to users
//    // check for name first
//    obj = FindObject(name);
//    if (obj != NULL)
//    {
//       InterpreterException ex
//          (type + " object named " + name + " already exist.");
//       HandleError(ex);
//       return obj;
//    }
   
   if (type == "Spacecraft") 
      obj = (GmatBase*)theModerator->CreateSpacecraft(type, name);
   
   else if (type == "Formation") 
      obj = (GmatBase*)theModerator->CreateSpacecraft(type, name);
   
   else if (type == "PropSetup") 
      obj = (GmatBase*)theModerator->CreatePropSetup(name);
   
   else if (type == "ForceModel") 
      obj = (GmatBase*)theModerator->CreateForceModel(name);
   
   else if (type == "CoordinateSystem") 
      obj = (GmatBase*)theModerator->CreateCoordinateSystem(name);
   
   else
   {
      // Handle Propagator
      if (obj == NULL)
         if (find(propagatorList.begin(), propagatorList.end(), type) != 
             propagatorList.end()) 
         {
            obj = (GmatBase*)theModerator->CreatePropagator(type, name);
         }
      
      // Handle AxisSystem
      if (find(axisSystemList.begin(), axisSystemList.end(), type) != 
          axisSystemList.end()) 
      {
         obj =(GmatBase*) theModerator->CreateAxisSystem(type, name);
      }

      // Handle Atmosphere Model
      if (obj == NULL)
         if (find(atmosphereList.begin(), atmosphereList.end(), type) != 
             atmosphereList.end()) 
         {
            obj = (GmatBase*)theModerator->CreateAtmosphereModel(type, name);
         }
      
//       // Handle Attitude
//       if (obj == NULL)
//          if (find(attitudeList.begin(), attitudeList.end(), type) != 
//              attitudeList.end()) 
//          {
//             obj = (GmatBase*)theModerator->CreateAttitude(type, name);
//          }
      
      // Handle Burns
      if (obj == NULL)
         if (find(burnList.begin(), burnList.end(), type) != 
             burnList.end()) 
         {
            obj = (GmatBase*)theModerator->CreateBurn(type, name);
         }
      
      // Handle CalculatedPoint (Barycenter, LibrationPoint)
      if (obj == NULL)
         if (find(calculatedPointList.begin(), calculatedPointList.end(), type) != 
             calculatedPointList.end()) 
         {
            obj =(GmatBase*) theModerator->CreateCalculatedPoint(type, name);
         }
      
      // Handle Functions
      if (obj == NULL)
         if (find(functionList.begin(), functionList.end(), type) != 
             functionList.end()) 
         {
            obj = (GmatBase*)theModerator->CreateFunction(type, name);
         }
      
      // Handle Hardware (tanks, thrusters, etc.)
      if (obj == NULL)
         if (find(hardwareList.begin(), hardwareList.end(), type) != 
             hardwareList.end()) 
         {
            obj = (GmatBase*)theModerator->CreateHardware(type, name);
         }
      
      // Handle System Parameters
      if (obj == NULL)
         if (find(parameterList.begin(), parameterList.end(), type) != 
             parameterList.end())
         {
            obj = (GmatBase*)CreateParameter(type, name);
         }
      
      // Handle PhysicalModel
      if (obj == NULL)
         if (find(physicalModelList.begin(), physicalModelList.end(), type) != 
             physicalModelList.end())
         {
            obj = (GmatBase*)theModerator->CreatePhysicalModel(type, name);
         }
      
      // Handle Solvers
      if (obj == NULL)
         if (find(solverList.begin(), solverList.end(), type) != 
             solverList.end()) 
         {
            obj = (GmatBase*)theModerator->CreateSolver(type, name);
         }
      
      // Handle Subscribers
      if (obj == NULL)
         if (find(subscriberList.begin(), subscriberList.end(), type) != 
             subscriberList.end())
         {
            obj = (GmatBase*)theModerator->CreateSubscriber(type, name);
         }
   
   }
   
   #if DEBUG_CREATE_OBJECT
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
// bool FindPropertyID(GmatBase *obj, const std::string &chunk, GmatBase **owner,
//                     Integer &id)
//------------------------------------------------------------------------------
/*
 * Finds property ID for given property. If property not found in the obj,
 * it tries to find proerty from the owned objects.
 *
 * @param  obj    Object to find proerty
 * @param  chunk  String contains property
 * @param  owner  Address of new owner pointer to be returned
 * @param  id     Property ID to return
 *
 * @return true if property found
 *
 * For example, From "FM.Gravity.Earth.Model"
 *   obj is FM pointer, chunk is "Gravity.Earth.Model"
 */
//------------------------------------------------------------------------------
bool Interpreter::FindPropertyID(GmatBase *obj, const std::string &chunk,
                                 GmatBase **owner, Integer &id)
{
   if (obj == NULL)
      return false;
   
   bool retval = false;
   StringArray parts = theTextParser.SeparateDots(chunk);
   Integer count = parts.size();
   std::string prop = parts[count-1];
   id = -1; // Set initial output id to -1
   
   try
   {
      id = obj->GetParameterID(prop);
      *owner = obj;
      retval = true;
   }
   catch (BaseException &e)
   {
      if (FindOwnedObject(obj, prop, owner, id))
         retval = true;
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to find a configured object.
 *
 * @param  name  Name of the object.
 *
 * @return Configured object pointer
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::FindObject(const std::string &name)
{
   if (name == "SolarSystem")
      return theModerator->GetSolarSystemInUse();
   else
      return theModerator->GetObject(name);
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &type, const std::string &desc)
//------------------------------------------------------------------------------
GmatCommand* Interpreter::CreateCommand(const std::string &type,
                                        const std::string &desc)
{
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::CreateCommand() type=%s, desc=%s\n", type.c_str(),
       desc.c_str());
   #endif

   GmatCommand *cmd = NULL;
   std::string desc1 = desc;
   bool found = false;
   
   if (find(commandList.begin(), commandList.end(), type)
       != commandList.end())
      found = true;
   
   // Check for CallFunction
   if (type[0] == '[')
   {
      cmd = AppendCommand("CallFunction");
      desc1 = type + desc;
      cmd->SetGeneratingString(desc1);
   }
   /// @TODO: This is a work around for a call function with
   /// without any return parameters.  It should be updated in
   /// the design to handle this situation.
   else if ((desc1.find("=") == desc1.npos) && (desc != "")
            && (!found))
   {
      cmd = AppendCommand("CallFunction");
      desc1 = "[] =" + type + desc;
      cmd->SetGeneratingString(desc1);	
   }
   else
   {
      cmd = AppendCommand(type);
      cmd->SetGeneratingString(type + " " + desc);
   }
   
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage("   %s created.\n", cmd->GetTypeName().c_str());
   #endif
   
   try
   {
      // if command has it's own InterpretAction(), jsut return cmd
      if (cmd->InterpretAction())
      {
         #if DEBUG_ASSEMBLE_COMMAND
         MessageInterface::ShowMessage
            ("   ===> %s has InterpretAction() \n", type.c_str());
         #endif
         
         return cmd;
      }
   }
   catch (BaseException &e)
   {
      HandleError(e);
      return NULL;
   }
   
   // Assemble command
   if (desc1 != "")
      AssembleCommand(cmd, desc1);
   
   return cmd;;
}


//------------------------------------------------------------------------------
//GmatCommand* AppendCommand(const std::string &type)
//------------------------------------------------------------------------------
GmatCommand* Interpreter::AppendCommand(const std::string &type)
{
   return theModerator->AppendCommand(type, "");
}


//------------------------------------------------------------------------------
//bool AssembleCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleCommand(GmatCommand *cmd, const std::string &desc)
{
   bool retval = false;
   std::string type = cmd->GetTypeName();

   #if DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("Interpreter::AssembleCommand() cmd=%s\n   desc=%s\n",
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
//bool AssembleForCommand(GmatCommand *cmd, const std::string &desc)
//------------------------------------------------------------------------------
bool Interpreter::AssembleForCommand(GmatCommand *cmd, const std::string &desc)
{
   bool retval = true;
   UnsignedInt equalSign = desc.find("=");
   
   if (equalSign == desc.npos)
   {
      InterpreterException ex("Cannot find equal sign (=) ");
      HandleError(ex);
      return false;
   }
   
   std::string var = desc.substr(0, equalSign-1);
   var = GmatStringUtil::Trim(var);
   
   std::string substr = desc.substr(equalSign+1);
   
   StringArray parts = theTextParser.SeparateBy(substr, ":");
   int count = parts.size();
   
   if (count < 2)
   {
      InterpreterException ex("Cannot find equal sign (=) in ");
      HandleError(ex);
      return false;
   }
   
   std::string start = parts[0];
   std::string end = parts[1];
   std::string step = "1";
   
   if (count > 2)
      step = parts[2];
   
   #if DEBUG_ASSEMBLE_FOR
   MessageInterface::ShowMessage
      ("Interpreter::AssembleForCommand() var=%s, start=<%s>, end=<%s>, "
       "step=<%s>\n", var.c_str(), start.c_str(), end.c_str(), step.c_str());
   #endif
   
   cmd->SetStringParameter("IndexName", var);
   cmd->SetStringParameter("StartName", start);
   cmd->SetStringParameter("EndName", end);
   cmd->SetStringParameter("IncrementName", step);

   GmatBase *obj = FindObject(var);

   // Create index variable
   if (obj == NULL)
      obj = CreateObject("Variable", var);
   
   cmd->SetRefObject(obj, Gmat::PARAMETER, var);

   // Set start varibale, if exist
   obj = FindObject(start);
   if (obj != NULL)
      cmd->SetRefObject(obj, Gmat::PARAMETER, start);

   // Set end varibale, if exist
   obj = FindObject(end);
   if (obj != NULL)
      cmd->SetRefObject(obj, Gmat::PARAMETER, end);

   // Set step varibale, if exist
   obj = FindObject(step);
   if (obj != NULL)
      cmd->SetRefObject(obj, Gmat::PARAMETER, step);
   
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
   UnsignedInt index1 = desc.find("=");
   std::string lhs = desc.substr(0, index1);
   StringArray outArray;

   // only get out parameters if there was an equal sign
   if (index1 != lhs.npos)
   {   
      outArray = theTextParser.SeparateBrackets(lhs, "[]", " ,", true);
   }
   
   // Function Name, Input
   StringArray inArray;
   std::string funcName;
   UnsignedInt index2 = desc.find("(", index1);
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

   #if DEBUG_ASSEMBLE_CALL_FUNCTION
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
   
   #if DEBUG_ASSEMBLE_CALL_FUNCTION
   MessageInterface::ShowMessage("   Setting input\n");
   WriteParts("CallFunction Input" , inArray);
   #endif
   
   // Set input to CallFunction
   for (UnsignedInt i=0; i<inArray.size(); i++)
      retval = cmd->SetStringParameter("AddInput", inArray[i]);
   
   // Set output to CallFunction
   #if DEBUG_ASSEMBLE_CALL_FUNCTION
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
   
   // Remove enclosed parenthesis first
   Integer length = desc.size();
   std::string str1 = desc;
   if (desc[0] == '(' && desc[length-1] == ')')
      str1 = desc.substr(1, length-2);
   
   UnsignedInt start = 0;
   UnsignedInt right = 0;
   UnsignedInt op = 0;
   bool done = false;
   StringArray parts;
   std::string str2;
   
   // Parse conditions
   while (!done)
   {
      op = str1.find_first_of(opStr, start);
      if (op == str1.npos)
      {
         // Add final right of operator
         str2 = GmatStringUtil::Trim(str1.substr(start));
         parts.push_back(str2);
         break;
      }
      
      // Add left of operator
      str2 = GmatStringUtil::Trim(str1.substr(start, op-start));
      //parts.push_back(str1.substr(start, op-start));
      parts.push_back(str2);

      // Add operator
      right = str1.find_first_not_of(opStr, op);      
      str2 = GmatStringUtil::Trim(str1.substr(op, right-op));
      //parts.push_back(str1.substr(op, right-op));
      parts.push_back(str2);
      
      start = op + 1;
      op = str1.find_first_of(opStr, start);
      
      // check for double ops (such as: == ~= >= <=)
      if (op != str1.npos && op == start)
         start = op + 1;
   }
   
   #if DEBUG_ASSEMBLE_COMMAND
   WriteParts("After parsing conditions()" , parts);
   #endif
   
   Integer count = parts.size();
   
   // assuming there is no boolean argument
   if (count < 3 || ((count-3)%4) != 0)
   {
      InterpreterException ex
         ("The Command has invalid number of conditions in " + type + " " +
          desc + "\n");
      HandleError(ex);
      return false;
   }
   
   ConditionalBranch *cb = (ConditionalBranch*)cmd;
   
   for (int i=0; i<count; i+=4)
   {
      #if DEBUG_ASSEMBLE_COMMAND
      MessageInterface::ShowMessage
         ("   lhs:%s, op:%s, rhs:%s\n", parts[i].c_str(), parts[i+1].c_str(),
          parts[i+2].c_str());
      #endif
      
      // Try to create a parameter first
      CreateParameter(parts[i]);
      CreateParameter(parts[i+2]);
      
      cb->SetCondition(parts[i], parts[i+1], parts[i+2]);
      
      if (count > i+3)
      {
         #if DEBUG_ASSEMBLE_COMMAND
         MessageInterface::ShowMessage("   logOp=%s\n", parts[i+3].c_str());
         #endif
         
         cb->SetConditionOperator(parts[i+3]);
      }
   }
   
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
   StringArray parts = theTextParser.Decompose(desc, "()");
   Integer count = parts.size();
   
   #if DEBUG_ASSEMBLE_COMMAND
   WriteParts("AssembleGeneralCommand()" , parts);
   #endif
   
   if (type == "Target" || type == "Report" || type == "BeginFiniteBurn" ||
       type == "EndFiniteBurn" || type == "Optimize")
   {
      // first item is ref. object name
      
      if (type == "Target")
      {
         cmd->SetRefObjectName(Gmat::SOLVER, parts[0]);
         
         for (int i=1; i<count; i++)
            cmd->SetRefObjectName(Gmat::PARAMETER, parts[i]);
      }
      else if (type == "Optimize")
      {
         cmd->SetRefObjectName(Gmat::SOLVER, parts[0]);
         
         for (int i=1; i<count; i++)
            cmd->SetRefObjectName(Gmat::PARAMETER, parts[i]);
      }
      else if (type == "Report")
      {
         parts = theTextParser.SeparateBrackets(desc, "()", " ,", false);
         
         // first item is ReportFile
         GmatBase *obj = FindObject(parts[0]);
         
         if (obj != NULL)
         {
            cmd->SetRefObject(obj, Gmat::SUBSCRIBER, parts[0], 0);
            
            // next items are Parameters
            for (int i=1; i<count; i++)
            {
               obj = (GmatBase*)CreateParameter(parts[i]);
               if (obj != NULL)
               {
                  cmd->SetRefObject(obj, Gmat::PARAMETER, parts[i], 0);
               }
               else
               {
                  InterpreterException ex
                     ("Cannot find Report Variable: " + parts[i] +
                      ".\nIt will not be added to Report");
                  HandleError(ex);
               }
            }
         }
         else
         {
            InterpreterException ex
               ("*** ERROR *** Cannot find ReportFile: " + parts[0] +
                " in line:\n\"" + currentBlock + "\"\n");
            HandleError(ex);
         }
      }
      else
      {
         // Begin/EndFiniteBurn has the syntax: BeginFiniteBurn burn1(sat1 sat2)
         
         // Get FiniteBurn name
         parts = theTextParser.Decompose(desc, "()", false);
         
         #if DEBUG_ASSEMBLE_COMMAND
         WriteParts(type, parts);
         #endif
         
         cmd->SetRefObjectName(Gmat::BURN, parts[0]);
         
         // Get Spacecraft names
         StringArray subParts = theTextParser.SeparateBrackets(parts[1], "()", " ,");
         
         #if DEBUG_ASSEMBLE_COMMAND
         WriteParts(type, subParts);
         #endif
         
         count = subParts.size();
         for (int i=0; i<count; i++)
            cmd->SetRefObjectName(Gmat::SPACECRAFT, subParts[i]);
      }
   }
   else if (type == "Save")
   {
      for (int i=0; i<count; i++)
         cmd->SetRefObjectName(Gmat::SPACECRAFT, parts[i]);
   }
   else
   {
      retval = false;
   }
   
   #if DEBUG_ASSEMBLE_COMMAND
   MessageInterface::ShowMessage
      ("   AssembleGeneralCommand() completed setting %s\n", type.c_str());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
//GmatCommand* CreateAssignmentCommand(const std::string &lhs,
//                                     const std::string &rhs)
//------------------------------------------------------------------------------
GmatCommand* Interpreter::CreateAssignmentCommand(const std::string &lhs,
                                                  const std::string &rhs)
{
   std::string desc = lhs + " = " + rhs;
   return CreateCommand("GMAT", desc);
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Creates Parameter from the parameter string if it has a valid Parameter Type.
 */
//------------------------------------------------------------------------------
Parameter* Interpreter::CreateParameter(const std::string &str)
{
   #if DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("===> Interpreter::CreateParameter() str=%s\n", str.c_str());
   #endif
   
   Parameter *param = NULL;
   std::string paramType, ownerName, depName;
   GmatStringUtil::ParseParameter(str, paramType, ownerName, depName);

   // Create parameter, if type is System Parameter, Variable, Array
   if (find(parameterList.begin(), parameterList.end(), paramType) != 
       parameterList.end())
   {
      //param = CreateParameter(paramType, str, depName, ownerName);
      param = CreateParameter(paramType, str, ownerName, depName);
   }
   else
   {
      param = (Parameter*)FindObject(str);
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
   #if DEBUG_CREATE_PARAM
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

   #if DEBUG_CREATE_ARRAY
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
   
   #if DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("Interpreter::GetArrayIndex() arrayStr=%s, name=%s, rowStr=%s, "
       "colStr=%s, row=%d, col=%d\n", arrayStr.c_str(), name.c_str(),
       rowStr.c_str(), colStr.c_str(), row, col);
   #endif

   GmatBase *obj = FindObject(name);
   
   if (obj == NULL)
   {
      InterpreterException ex
         ("Interpreter::GetArrayIndex() cannot find Array name: " + name + "\n");
      HandleError(ex);
   }
   
   if (obj->GetTypeName() != "Array")
   {
      InterpreterException ex
         ("Interpreter::GetArrayIndex() " + name + " is not an Array\n");
      HandleError(ex);
   }
   
   // get row value
   if (row == -1 && rowStr != "-1")
   {
      Parameter *param = (Parameter*)FindObject(rowStr);
      if (param == NULL)
      {
         InterpreterException ex
            ("Interpreter::GetArrayIndex() cannot find Array row index "
             "Variable: " + name + "\n");
         HandleError(ex);
      }
      
      if (param->GetReturnType() == Gmat::REAL_TYPE)
         row = (Integer)param->GetReal() - 1; // index starts at 0
      else
         if (param == NULL)
         {
            InterpreterException ex
               ("Interpreter::GetArrayIndex() cannot handle Array row of "
                "Array: " + name + "\n");
            HandleError(ex);
         }
   }
   
   
   // get column value
   if (col == -1 && colStr != "-1")
   {
      Parameter *param = (Parameter*)FindObject(colStr);
      if (FindObject(colStr) == NULL)
      {
         InterpreterException ex
            ("Interpreter::GetArrayIndex() cannot find Array column index "
             "Variable: " + name + "\n");
         HandleError(ex);
      }
      
      if (param->GetReturnType() == Gmat::REAL_TYPE)
         col = (Integer)param->GetReal() - 1; // index starts at 0
      else
         if (param == NULL)
         {
            InterpreterException ex
               ("Interpreter::GetArrayIndex() cannot handle Array column of "
                "Array: " + name + "\n");
            HandleError(ex);
         }
   }
   
   if (row == -1)
   {
      InterpreterException ex
         ("Interpreter::GetArrayIndex() Row index is invalid\n");
      HandleError(ex);
   }
   
   if (col == -1)
   {
      InterpreterException ex
         ("Interpreter::GetArrayIndex() Column index is invalid\n");
      HandleError(ex);
   }

   #if DEBUG_ARRAY_GET
   MessageInterface::ShowMessage
      ("   GetArrayIndex() row=%d, col=%d\n", row, col);
   #endif
   
   return (Parameter*)obj;
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
   UnsignedInt dot;
   std::string lhsObjName, rhsObjName;
   std::string lhsPropName, rhsPropName;
   GmatBase *lhsObj = NULL;
   GmatBase *rhsObj = NULL;
   bool isLhsObject = false;
   bool isRhsObject = false;
   bool isLhsArray = false;
   bool isRhsArray = false;
   currentBlock = lhs + " = " + rhs;
   
   #if DEBUG_ASSIGNMENT
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
         InterpreterException ex
            ("Interpreter::MakeAssignment() Cannot find LHS object: " + lhsObjName + "\n");
         HandleError(ex);
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
         if (IsArray(lhs))
            isLhsArray = true;
         else
            isLhsObject = true;
      }
      else
      {
         InterpreterException ex("Cannot find LHS object: " + lhs + "\n");
         HandleError(ex);
      }
   }
   
   #if DEBUG_ASSIGNMENT
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
         
         #if DEBUG_ASSIGNMENT
         MessageInterface::ShowMessage
            ("   Cannot find RHS object: %s. It may be a string value\n",
             rhsObjName.c_str());
         #endif
      }
      else
      {
         dot = rhs.find('.');
         if (dot == rhs.npos)
            rhsPropName = rhsParts[1];
         else
            rhsPropName = rhs.substr(dot+1);
      }
   }
   else
   {
      // if rist RHS char is - sign, use without it in finding name
      std::string newName = rhs;
      
      if (rhs[0] == '-')
         newName = rhs.substr(1);
      
      rhsObj = FindObject(newName);

      if (rhsObj)
      {
         if (IsArray(rhs))
            isRhsArray = true;
         else
            isRhsObject = true;
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
         ("Interpreter::MakeAssignment() Error if it reached here.\n");
      HandleError(ex);
   }
   
   #if DEBUG_ASSIGNMENT
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
   #if DEBUG_SET
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
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("SetPropertyToObject() toObj=%s, fromOwner=%s, fromProp=%s\n",
       toObj->GetName().c_str(), fromOwner->GetName().c_str(), fromProp.c_str());
   #endif
   
   if (toObj->GetTypeName() != "Variable" && toObj->GetTypeName() != "String")
   {
      InterpreterException ex
         ("Cannot set property to object other than Variable or String.");
      HandleError(ex);
      return false;
   }
   
   Integer fromId = fromOwner->GetParameterID(fromProp);
   Gmat::ParameterType fromType = fromOwner->GetParameterType(fromId);
   Gmat::ParameterType toType = ((Parameter*)toObj)->GetReturnType();
   
   if (fromType == toType)
   {
      if (toType == Gmat::STRING_TYPE)
         toObj->SetStringParameter("Value", fromOwner->GetStringParameter(fromId));
      else if (toType == Gmat::REAL_TYPE)
         toObj->SetRealParameter("Value", fromOwner->GetRealParameter(fromId));
   }
   else
   {
      InterpreterException ex
         ("Cannot set property to object. Parameter types are not the same.");
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
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToObject() toObj=%s, fromArray=%s\n",
       toObj->GetName().c_str(), fromArray.c_str());
   #endif
   
   if (toObj->GetTypeName() != "Variable")
   {
      InterpreterException ex
         ("Cannot set Array element to object other than Variable.");
      HandleError(ex);
      return false;
   }
   
   Integer row, col;
   GetArrayIndex(fromArray, row, col);
   
   #if DEBUG_SET
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
   
   #if DEBUG_SET
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
   
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetValueToObject() objType=%s, toObj=%s, value=%s\n",
       objType.c_str(), toObj->GetName().c_str(), value.c_str());
   #endif
   
   if (objType != "Variable" && objType != "String")
   {
      InterpreterException ex
         ("Cannot set value to object other than Variable or String.");
      HandleError(ex);
      return false;
   }
   
   if (objType == "String")
   {
      #if DEBUG_SET
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
         if (GmatStringUtil::ToDouble(value, rval))
         {      
            #if DEBUG_SET
            MessageInterface::ShowMessage("   SetValueToObject() rval=%f\n", rval);
            #endif
         
            toObj->SetRealParameter("Value", rval);
         }
         else
         {
            toObj->SetStringParameter(toObj->GetParameterID("Expression"), value);
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
   #if DEBUG_SET
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
            ("Cannot set Object: " + objName + " to ForceModel: " +
             toOwner->GetName() + "\n");
         HandleError(ex);
         return false;
      }
   }
   else
   {
      GmatBase *toObj = NULL;
      Integer toId = -1;

      try
      {
         FindPropertyID(toOwner, toProp, &toObj, toId);
      
         if (toObj == NULL)
         {
            InterpreterException ex
               ("*** Internal Error *** "
                "Interpreter::SetObjectToProperty() toObj is NULL\n");
            HandleError(ex);
         }
      }
      catch (BaseException &e)
      {
         delayedBlocks.push_back(currentBlock);
         
         #if DEBUG_SET
         MessageInterface::ShowMessage
            ("   ===> added to delayed blocks: %s\n", currentBlock.c_str());
         #endif
         
         return true;
      }
      
      Gmat::ParameterType toType = toObj->GetParameterType(toId);
      
      #if DEBUG_SET
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
         else
         {
            if (toType == Gmat::STRING_TYPE)
            {
               toObj->SetStringParameter(toId, fromObj->GetName());
            }
            else
            {
               InterpreterException ex
                  ("Cannot set Variable or String to property. "
                   "Parameter types are not the same.");
               HandleError(ex);
               return false;
            }
         }
      }
      else
      {
         #if DEBUG_SET
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
   
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("SetPropertyToProperty() toOwner=%s, toProp=%s, fromOwner=%s, fromProp=%s\n",
       toOwner->GetName().c_str(), toProp.c_str(), fromOwner->GetName().c_str(),
       fromProp.c_str());
   #endif
   
   Integer toId = -1;
   Gmat::ParameterType toType = Gmat::UNKNOWN_PARAMETER_TYPE;
   std::string lhs = toOwner->GetName() + "." + toProp;
   std::string rhs = fromOwner->GetName() + "." + fromProp;
   std::string value;
   Parameter *lhsParam = NULL;
   Parameter *rhsParam = NULL;
   
   #if DEBUG_SET
   MessageInterface::ShowMessage("   lhs=%s, rhs=%s\n", lhs.c_str(), rhs.c_str());
   #endif

   //-----------------------------------
   // try LHS property
   //-----------------------------------
   
   try
   {
      toId = toOwner->GetParameterID(toProp);
      toType = toOwner->GetParameterType(toId);
   }
   catch (BaseException &e)
   {
      lhsParam = CreateParameter(lhs);
   }
   
   //-----------------------------------
   // try RHS property
   //-----------------------------------
   // try create parameter first if lhs type is OBJECT_TYPE
   if (toType == Gmat::OBJECT_TYPE)
      rhsParam = CreateParameter(rhs);
   
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
   
   
   #if DEBUG_SET
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
         retval = SetProperty(toOwner, toId, value);
      }
      else
      {
         retval = SetProperty(toOwner, toId, rhs);
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
               retval = SetPropertyValue(toOwner, toId, value);
            }
            else
            {
               retval = SetPropertyValue(toOwner, toId, rhs);
            }
         }
         else
         {
            value = GetPropertyValue(fromOwner, fromId);
            retval = SetProperty(toOwner, toId, value);
         }
      }
      else
      {
         retval = SetProperty(toOwner, toId, rhs);
      }
   }
   
   if (!retval)
   {
      InterpreterException ex
         ("Cannot set property: " + fromProp + " to property: " + toProp);
      HandleError(ex);
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
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToProperty() toOwner=%s, toProp=%s, fromArray=%s\n",
       toOwner->GetName().c_str(), toProp.c_str(), fromArray.c_str());
   #endif
   
   Integer toId = toOwner->GetParameterID(toProp);
   Gmat::ParameterType toType = toOwner->GetParameterType(toId);

   if (toType != Gmat::REAL_TYPE)
   {
      InterpreterException ex
         ("Cannot set Array element to property. Property type is not Real.");
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
   
   #if DEBUG_SET
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
   errorMsg = "";
   
   #if DEBUG_SET
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
         
         if (FindPropertyID(toOwner, toProp, &toObj, toId))
            retval = SetProperty(toObj, toId, value);
         
      }
   }
   
   if (retval == false)
   {
      InterpreterException ex
         (errorMsg + "Cannot set " + toOwner->GetTypeName() + " property: " + toProp);
      HandleError(ex);
   }
   
   #if DEBUG_SET
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
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetObjectToArray() toArrObj=%s, toArray=%s, fromObj=%s\n",
       toArrObj->GetName().c_str(), toArray.c_str(), fromObj->GetName().c_str());
   #endif
   
   if (fromObj->GetTypeName() != "Variable")
   {
      InterpreterException ex
         ("Cannot set object other than Variable or Array element.");
      HandleError(ex);
      return false;
   }
   
   Real rval = fromObj->GetRealParameter("Value");
   
   Integer row, col;
   GetArrayIndex(toArray, row, col);
   
   #if DEBUG_SET
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
   #if DEBGU_SET
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
         ("Cannot set property to Array element. Parameter type is not Real.");
      HandleError(ex);
      return false;
   }
   
   Real rval = fromOwner->GetRealParameter(fromId);
   
   Integer row, col;
   GetArrayIndex(toArray, row, col);

   #if DEBUG_SET
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
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetArrayToArray() toArrObj=%s, toArray=%s, "
       "fromArrObj=%s, fromArray=%s\n", toArrObj->GetName().c_str(),
       toArray.c_str(), fromArrObj->GetName().c_str(), fromArray.c_str());
   #endif
   
   Integer rowFrom, colFrom;
   Integer rowTo, colTo;
   
   GetArrayIndex(toArray, rowTo, colTo);
   GetArrayIndex(fromArray, rowFrom, colFrom);
   
   Real rval = GetArrayValue(fromArray, rowFrom, colFrom);
   
   #if DEBUG_SET
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
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetValueToArray() array=%s, toArray=%s, value=%s\n",
       array->GetName().c_str(), toArray.c_str(), value.c_str());
   #endif
   
   Integer row, col;
   Real rval;

   GetArrayIndex(toArray, row, col);
   
   
   if (GmatStringUtil::ToDouble(value, rval))
   {
      #if DEBUG_SET
      MessageInterface::ShowMessage
         ("   SetValueToArray() rval=%f, row=%d, col=%d\n", rval, row, col);
      #endif

      try
      {
         array->SetRealParameter("SingleValue", rval, row, col);
      }
      catch (BaseException &e)
      {
         InterpreterException ex("Out of bound index found: " + toArray);
         HandleError(ex);
         return false;
      }
   }
   else
   {
      InterpreterException ex("Invalid real number found: " + value);
      HandleError(ex);
      return false;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool SetPropertyValue(GmatBase *obj, const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets parameters on GMAT objects.
 * 
 * @param  obj    Pointer to the object that owns the parameter.
 * @param  id     ID for the parameter.
 * @param  value  Value of the parameter.
 * 
 * @return true if the parameter is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Interpreter::SetPropertyValue(GmatBase *obj, const Integer id,
                                   const std::string &value)
{
   bool retval = false;
   
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetPropertyValue() obj=%s, id=%d, value=%s\n",
       obj->GetName().c_str(), id, value.c_str());
   #endif
   
   std::string valueToUse = value;
   CheckForSpecialCase(obj, id, valueToUse);
   
   Gmat::ParameterType type = obj->GetParameterType(id);
   
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("   paramType=%s\n", GmatBase::PARAM_TYPE_STRING[type].c_str());
   #endif
   
   
   switch (type)
   {
   case Gmat::OBJECT_TYPE:
   case Gmat::OBJECTARRAY_TYPE:
      {
         // Try create Parameter first
         Parameter *param = CreateParameter(value);
         
         #if DEBUG_SET
         if (param)
            MessageInterface::ShowMessage
               ("   param=%s\n", param->GetName().c_str());
         #endif
         
         if (param != NULL)
         {
            #if DEBUG_SET
            MessageInterface::ShowMessage
               ("===> Calling SetStringParameter(%d, %s)\n", id, value.c_str());
            #endif
            
            obj->SetStringParameter(id, value);
         }
         else
         {
            // Create Owned Object
            GmatBase *ownedObj = CreateObject(value, "");
            
            #if DEBUG_SET
            if (ownedObj)
               MessageInterface::ShowMessage
                  ("   Created ownedObjType: %s\n",
                   ownedObj->GetTypeName().c_str());
            #endif
            
            if (ownedObj)
            {
               #if DEBUG_SET
               MessageInterface::ShowMessage
                  ("===> Calling SetRefObject(%p, %d)\n", ownedObj,
                   ownedObj->GetType());
               #endif
               
               obj->SetRefObject(ownedObj, ownedObj->GetType());
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
                     InterpreterException ex
                        ("*** Internal Error ***Cannot create " + value + "\n");
                     HandleError(ex);
                  }
               }
            }
         }
         
         retval = true;
         break;
      }
   case Gmat::INTEGER_TYPE:
   case Gmat::UNSIGNED_INT_TYPE:
   case Gmat::UNSIGNED_INTARRAY_TYPE:
      {
         Integer ival;
         if (GmatStringUtil::ToInteger(valueToUse, ival))
         {
            #if DEBUG_SET
            MessageInterface::ShowMessage
               ("===> Calling SetIntegerParameter(%d, %d)\n", id, ival);
            #endif
            
            obj->SetIntegerParameter(id, ival);
            retval = true;
         }
         else
         {
            errorMsg = errorMsg + valueToUse + " is not a valid Integer number.\n";
         }
         break;
      }
   case Gmat::REAL_TYPE:
      {
         Real rval;
         if (GmatStringUtil::ToDouble(valueToUse, rval))
         {
            #if DEBUG_SET
            std::string rvalStr =
               GmatStringUtil::ToString(rval, false, 17, 16);
            MessageInterface::ShowMessage
               ("===> Calling SetRealParameter(%d, %s)\n", id, rvalStr.c_str());
            #endif
            
            obj->SetRealParameter(id, rval);
            retval = true;
         }
         else
         {
            errorMsg = errorMsg + valueToUse + " is not a valid Real number.\n";
         }
         break;
      }
   case Gmat::STRING_TYPE:
   case Gmat::STRINGARRAY_TYPE:
      {
         #if DEBUG_SET
         MessageInterface::ShowMessage
            ("===> Calling SetStringParameter(%d, %s)\n", id, valueToUse.c_str());
         #endif
         
         retval = obj->SetStringParameter(id, valueToUse);
         break;
      }
   case Gmat::BOOLEAN_TYPE:
      {
         bool tf;
         if (GmatStringUtil::ToBoolean(value, tf))
         {
            #if DEBUG_SET
            MessageInterface::ShowMessage
               ("===> Calling SetBooleanParameter(%d, %d)\n", id, tf);
            #endif
            
            obj->SetBooleanParameter(id, tf);
            retval = true;
         }
         break;
      }
   default:
      InterpreterException ex
         ("Interpreter::SetPropertyValue() Cannot handle the type: " +
          GmatBase::PARAM_TYPE_STRING[type] + " yet.\n");
      HandleError(ex);
   }
   
   #if DEBUG_SET
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
   
   return sval;
}


//------------------------------------------------------------------------------
// bool SetProperty(GmatBase *obj, const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets parameters on GMAT objects.
 * 
 * @param  obj    Pointer to the object that owns the parameter.
 * @param  id     ID for the parameter.
 * @param  value  Value of the parameter.
 * 
 * @return true if the parameter is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Interpreter::SetProperty(GmatBase *obj, const Integer id,
                              const std::string &value)
{
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("Interpreter::SetProperty() obj=%s, id=%d, value=%s\n", obj->GetName().c_str(),
       id, value.c_str());
   #endif
   
   bool retval = false;
   
   std::string valueToUse = value;
   CheckForSpecialCase(obj, id, valueToUse);
   
   #if DEBUG_SET
   MessageInterface::ShowMessage("   paramType=%d\n", obj->GetParameterType(id));
   #endif
   
   StringArray rhsValues;
   Integer count = 0;
   
   // if value has braces, setting multiple values
   if (value.find("{") != value.npos)
      rhsValues = theTextParser.SeparateBrackets(value, "{}", " ,");
   
   count = rhsValues.size();
   
   if (count > 0)
   {
      for (int i=0; i<count; i++)
         retval = SetPropertyValue(obj, id, rhsValues[i]);
   }
   else
   {
      retval = SetPropertyValue(obj, id, value);
   }
   
   #if DEBUG_SET
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
   #if DEBUG_SET
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

   // Current ForceModel scripting
   //GMAT FM.PrimaryBodies        = {Earth};
   //GMAT FM.PointMasses          = {Sun, Luna, Jupiter}; 
   //GMAT FM.Drag                 = None;
   //GMAT FM.Gravity.Earth.Model  = JGM2;
   //GMAT FM.Gravity.Earth.Degree = 20;
   //GMAT FM.Gravity.Earth.Order  = 20;
   //GMAT FM.SRP                  = On;
   
   // For future scripting we want to specify body for Drag and SRP
   // eg. FM.Drag.Earth = JacchiaRoberts;
   //     FM.Drag.Mars = MarsAtmos;
   //     FM.SRP.ShadowBodies = {Earth,Moon}
   
   ForceModel *forceModel = (ForceModel*)obj;
   std::string forceType = ForceModel::GetScriptAlias(pmType);
   std::string centralBodyName = forceModel->GetStringParameter("CentralBody");
   
   #if DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("Interpreter::SetForceModelProperty() fm=%s, prop=%s, value=%s\n"
       "   pmType=%s, forceType=%s\n", obj->GetName().c_str(), prop.c_str(), value.c_str(),
       pmType.c_str(), forceType.c_str());
   #endif
   
   //------------------------------------------------------------
   // Create ForceModel owned PhysicalModel
   //------------------------------------------------------------
   
   if (pmType == "PrimaryBodies" || pmType == "PointMasses")
   {
      StringArray bodies = theTextParser.SeparateBrackets(value, "{}", " ,");
      
      for (UnsignedInt i=0; i<bodies.size(); i++)
      {
         #if DEBUG_SET_FORCE_MODEL
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
                                    
            #if DEBUG_SET_FORCE_MODEL
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
               retval = SetPropertyValue(pm, pm->GetParameterID("Model"), "JGM2");
            
         }
      }
      
      return true;
   }
   else if (pmType == "SRP" || pmType == "Drag")
   {
      if (pmType == "SRP" && value != "On")
         return true;
      
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
               ("Unable to set AtmosphereModel for drag force.");
            HandleError(ex);
         }
         
         /// @todo Add the body name for drag at other bodies
         if (value != "BodyDefault")
         {
            pm->SetStringParameter("BodyName", centralBodyName);
            GmatBase *am = CreateObject(value, "");
            if (am)
               pm->SetRefObject(am, Gmat::ATMOSPHERE);
            else
            {
               InterpreterException ex
                  ("Unable to create AtmosphereModel: " + value + " for drag force.");
               HandleError(ex);
            }
         }
      }
      else if (pmType == "SRP")
      {
         // Should we set SRP on ForceModel central body?
         pm->SetStringParameter("BodyName", centralBodyName);
      }
            
      #if DEBUG_SET_FORCE_MODEL
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
   
   #if DEBUG_SET_FORCE_MODEL
   MessageInterface::ShowMessage
      ("   Setting pmType=%s, forceType=%s, numForces=%d, propName=%s\n", pmType.c_str(),
       forceType.c_str(), numForces, propName.c_str());
   #endif
   
   GmatBase *owner;
   Integer propId;
   
   if (FindPropertyID(forceModel, propName, &owner, propId))
   {
      retval = SetPropertyValue(owner, owner->GetParameterID(propName), value);
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
   #if DEBUG_SET_SOLAR_SYS
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
      
         #if DEBUG_SET_SOLAR_SYS
         for (StringArray::iterator i = ephems.begin(); i != ephems.end(); ++i)
            MessageInterface::ShowMessage("   Source = %s\n", i->c_str());
         #endif
         
         theModerator->SetPlanetarySourceTypesInUse(ephems);
         retval = true;
      }
      else
      {
         Integer id = obj->GetParameterID(prop);
         retval = SetPropertyValue(obj, id, value);
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
      
      #if DEBUG_SET_SOLAR_SYS
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
         retval = SetPropertyValue(body, id, value);
      }
      catch (BaseException &e)
      {
         HandleError(e);
      }
   }
   
   #if DEBUG_SET_SOLAR_SYS
   MessageInterface::ShowMessage
      ("Interpreter::SetSolarSystemProperty() prop=%s, retval=%d\n",
       prop.c_str(), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool FindOwnedObject(GmatBase *owner, const std::string toProp,
//                      GmatBase **ownedObj, Integer &id)
//------------------------------------------------------------------------------
/*
 * Finds owned object and its property.
 *
 * @param  owner    Owner object to find owned object for property
 * @param  toProp   Property name to find
 * @param  id       Output owned property id (-1 if property not found)
 *
 * @return  true if property found from the owned object
 */
//------------------------------------------------------------------------------
bool Interpreter::FindOwnedObject(GmatBase *owner, const std::string toProp,
                                  GmatBase **ownedObj, Integer &id)
{
   #if DEBUG_SET
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

   #if DEBUG_SET
   MessageInterface::ShowMessage("   ownedObjCount=%d\n", ownedObjCount);
   #endif
   
   if (ownedObjCount > 0)
   {
      for (int i=0; i<ownedObjCount; i++)
      {
         tempObj = owner->GetOwnedObject(i);
         if (ownedObj)
         {
            #if DEBUG_SET
            MessageInterface::ShowMessage
               ("   i=%d, ownedObj type=<%s>, name=<%s>\n", i,
                tempObj->GetTypeName().c_str(), tempObj->GetName().c_str());
            #endif
            
            try
            {
               id = tempObj->GetParameterID(toProp);
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
         InterpreterException ex
            ("No parameter defined with description " + toProp + "on " +
             owner->GetName());
         HandleError(ex);
      }
   }
   
   #if DEBUG_SET
   MessageInterface::ShowMessage
      ("   FindOwnedObject() returning retval=%d, ownedObj=%p\n",
       retval, *ownedObj);
   #endif
   
   return retval;
   
}


// //------------------------------------------------------------------------------
// // bool SetArrayValue(GmatBase *obj, const std::string &arrayStr, Real rval)
// //------------------------------------------------------------------------------
// bool Interpreter::SetArrayValue(GmatBase *obj, const std::string &arrayStr,
//                                 Real rval)
// {
//    Integer row, col;
   
//    Parameter *param = GetArrayIndex(arrayStr, row, col);
   
//    if (row != -1 && col != -1)
//    {
//       param->SetRealParameter("SingleValue", rval, row-1, col-1);
//    }
//    else
//    {
//       InterpreterException ex("Invalid row and column index\n");
//       HandleError(ex);
//       return false;
//    }

//    return true;
// }


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
   #if DEBUG_SET
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
// bool IsArray(const std::string &str)
//------------------------------------------------------------------------------
bool Interpreter::IsArray(const std::string &str)
{
   bool retval = false;
   
   if (str.find("[") != str.npos)
   {
      InterpreterException ex("[ is invalid in assignment : " + str + "\n");
      HandleError(ex);
   }
   
   retval = GmatStringUtil::IsParenPartOfArray(str);
   
   #if DEBUG_ARRAY
   MessageInterface::ShowMessage
      ("Interpreter::IsArray() str=%s, array=%d\n", str.c_str(), retval);
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
// void HandleError(BaseException &e)
//------------------------------------------------------------------------------
void Interpreter::HandleError(BaseException &e)
{
   if (continueOnError)
   {
      std::string msg = "*** WARNING *** " + e.GetMessage() + " in line:\n   \"" +
         currentBlock + "\"\n";
      
      errorList.push_back(msg);
      //MessageInterface::ShowMessage(msg);
   }
   else
   {
      // Get the line number of the logical block
      lineNumber = itoa(theReadWriter->GetLineNumber(), 10);
      throw InterpreterException
         (e.GetMessage() + " in line\n   \"" + lineNumber + ":" + currentBlock + "\"\n");
   }
}


//------------------------------------------------------------------------------
// bool FinalPass()
//------------------------------------------------------------------------------
/**
 * Finishes up the Interpret call by setting internal references that are needed 
 * by the GUI.
 *
 * @return true if the references were set; false otherwise.
 */
//------------------------------------------------------------------------------
bool Interpreter::FinalPass()
{
   StringArray csList = 
      theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   
   CoordinateSystem *cs;
   
   for (StringArray::iterator i = csList.begin(); i != csList.end(); ++i)
   {
      #ifdef DEBUG_FINAL_PASS
      MessageInterface::ShowMessage("Setting up CoordinateSystem '%s'\n",
                                    i->c_str());
      #endif
      cs = (CoordinateSystem*)GetObject(*i);
      cs->Initialize();
   }

   return true;
}

//------------------------------------------------------------------------------
// std::string Interpreter::itoa(int value, unsigned int base)
//------------------------------------------------------------------------------
/**
 * C++ version std::string style "itoa":
 */
//------------------------------------------------------------------------------
std::string Interpreter::itoa(Integer value, unsigned int base)
{	
	const char digitMap[] = "0123456789abcdef";	
	std::string buf;

	// Guard:
	if (base == 0 || base > 16)
	{
		// Error: may add more trace/log output here
		return buf;
	}
	
	// Take care of negative int:
	std::string sign;
	int _value = value;

	// Check for case when input is zero:
	if (_value == 0) return "0";
	
	// Translating number to string with base:
	for (int i = 30; _value && i ; --i) 
	{
		buf = digitMap[ _value % base ] + buf;
		_value /= base;
	}
	return sign.append(buf);
}



