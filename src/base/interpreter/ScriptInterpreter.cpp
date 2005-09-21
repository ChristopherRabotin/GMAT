//$Header$
//------------------------------------------------------------------------------
//                               ScriptInterpreter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/09/11
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class implementation for the ScriptInterpreter
 */
//------------------------------------------------------------------------------


#include "ScriptInterpreter.hpp" // class's header file
#include "Moderator.hpp" // class's header file
#include "Command.hpp" //loj:added
#include <fstream>

// Maybe put something like this in the Gmat namespace?
#define REV_STRING "Build 5.0, August 2005"


//#define DEBUG_SCRIPTINTERPRETER
//#define DEBUG_SCRIPT_READING_AND_WRITING
//#define DEBUG_PARAMETER_PARSING
//#define DEBUG_GMAT_LINE
//#define DEBUG_ARRAY_ASSIGNMENT

ScriptInterpreter *ScriptInterpreter::instance = NULL;


//------------------------------------------------------------------------------
// ScriptInterpreter* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor for the ScriptInterpreter singleton.
 * 
 * @return Pointer to the singleton.
 */
//------------------------------------------------------------------------------
ScriptInterpreter* ScriptInterpreter::Instance()
{
    if (!instance)
        instance = new ScriptInterpreter;
    return instance;
}


//------------------------------------------------------------------------------
// ScriptInterpreter()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
// class constructor
ScriptInterpreter::ScriptInterpreter()
{
    moderator = Moderator::Instance();
}


//------------------------------------------------------------------------------
// ~ScriptInterpreter()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ScriptInterpreter::~ScriptInterpreter()
{
}


//------------------------------------------------------------------------------
// bool Interpret(void)
//------------------------------------------------------------------------------
/**
 * Parses the input stream, line by line, into GMAT objects.
 * 
 * @return true if the stream parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Interpret()
{
   if (!initialized)
      Initialize();
        
   sequenceStarted = false;
   bool retval = ReadScript();

   if (retval)
      retval = FinalPass();
      
   return retval;       
}


//------------------------------------------------------------------------------
// bool Interpret(const std::string &scriptfile)
//------------------------------------------------------------------------------
/**
 * Parses the input stream from a file into GMAT objects.
 * 
 * @param scriptfile The name of the input file.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Interpret(const std::string &scriptfile)
{
    bool retval = false;
    
    filename = scriptfile;
    std::ifstream inFile(scriptfile.c_str());
    instream = &inFile;
    
    retval = Interpret();
    
    inFile.close();
    instream = NULL;
    
    return retval;
}


//------------------------------------------------------------------------------
// bool Build()
//------------------------------------------------------------------------------
/**
 * Writes the currently configured data to an output stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Build()
{
    if (!initialized)
        Initialize();
        
    return WriteScript();
}


//------------------------------------------------------------------------------
// bool Build(const std::string &scriptfile)
//------------------------------------------------------------------------------
/**
 * Writes the currently configured data to a file.
 * 
 * @param scriptfile Name of the output file.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Build(const std::string &scriptfile)
{
    bool retval = false;
    
    filename = scriptfile;

    std::ofstream outFile(scriptfile.c_str());
    outstream = &outFile;

    retval = Build();
    
    outFile.close();
    outstream = NULL;
    
    return retval;
}


//------------------------------------------------------------------------------
// bool ReadScript()
//------------------------------------------------------------------------------
/**
 * Reads a script from the input stream line by line and parses it.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ReadScript()
{
   branchDepth = 0;
    
   if (instream->fail() || instream->eof()) 
   {
      return false;
   }
    
   chunks.clear();
    
   while (!instream->eof()) 
   {
      if (!ReadLine())
         return false;
      if (!Parse())
         return false;
   }
    
   ConfigManager* cm = ConfigManager::Instance();
   cm->ConfigurationChanged(false);
   
   moderator->SetCommandsUnchanged();
   
   if (branchDepth != 0) 
   {
      // Clear the command sequence
      moderator->ClearCommandSeq();
      if (branchDepth > 0)
         MessageInterface::ShowMessage("ScriptInterpreter::ReadScript "
            "completed without terminating all branch commands\n");
      if (branchDepth < 0)
         MessageInterface::ShowMessage("ScriptInterpreter::ReadScript "
            "completed with more End commands than branch commands\n");
      return false;
   }
    
   return true;
}


//------------------------------------------------------------------------------
// bool ReadLine()
//------------------------------------------------------------------------------
/**
 * Reads a line from the input stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ReadLine()
{
    char charLine[4096] = "";
    
    instream->getline(charLine, 4095);
    line = charLine;
//    MessageInterface::ShowMessage("ScriptInterpreter::ReadLine \""
//                                  + line + "\"\n");
    return true;
}


//------------------------------------------------------------------------------
// bool Parse()
//------------------------------------------------------------------------------
/**
 * Builds or configures GMAT objects based on the current line of script.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Parse()
{
    // Determine what kind of line we have
    ChunkLine();
    
    // Process accordingly
    if (!chunks.empty())
    {
        std::vector<std::string*>::iterator phrase = chunks.begin();

        // First try for object creation
        if (**phrase == "Create")
        {
            // Instantiate the core object
            std::string type, name = "";
            ++phrase;
            type = **phrase;
            ++phrase;
            
            while (phrase != chunks.end())
            {
                name = **phrase;
                
                if ((name == ";") || (name[0] == '%'))
                   break;

                if (name[0] == '(')
                {
                   MessageInterface::ShowMessage(
                      "Unknown characters found in Create line; please check %s"
                      " in the line \n   \"%s\"\n"
                      "(Perhaps you meant square braces to size an array?)\n",
                      name.c_str(), line.c_str());
                   ++phrase;
                   continue;
                }

                if (!InterpretObject(type, name))
                   throw InterpreterException("Unable to create object " +
                                              name + " of type " + type +
                                              "\nScript text: \"" + line +
                                              "\"");
                ++phrase;
            }
        }
        // Next try for object parameter setup or assignment
        else if ((**phrase == "GMAT") && (!sequenceStarted))
        {

            #ifdef DEBUG_GMAT_LINE
               MessageInterface::ShowMessage("Decomposing GMAT line\n\"%s\"\n",
                                             line.c_str());
               for (std::vector<std::string *>::iterator c = chunks.begin();
                    c != chunks.end(); ++c)
                  MessageInterface::ShowMessage("   \"%s\"\n", (*c)->c_str());
            #endif

            // Look up related object(s)
            ++phrase;
            
            bool hasEquals = (line.find("=") != std::string::npos ? true : false);
            // Check to see if this is a function return
            if (((**phrase)[0] == '[') || !hasEquals)
            {
               // It's one or more return parameters, so line is a function call
               #ifdef DEBUG_GMAT_LINE
                  MessageInterface::ShowMessage(
                     "Sequence not started; hasEquals is %s\n%s",
                     (hasEquals ? "true" : "false"),
                     "This line is a function interface\n"
                     );
               #endif
               if (InterpretFunctionCall())
               {
                  sequenceStarted = true;
                  return true;
               }
               return false;
            }

            StringArray sar = SeparateDots(**phrase);
            std::string objName = sar[0];

            // Handle global objects -- currently only the SolarSystem
            if ( (objName == "SolarSystem")
                 // Add others here
               )
            {
               ++phrase;
               if ((**phrase) != "=")
                  throw InterpreterException("Line \"" + line + 
                     "\" is attempting to set a global parameter;" +
                     " an '=' is required\n");
               ++phrase;
               if (InterpretGlobalSetting(sar, (**phrase)))
               {
                  chunks.clear();
                  return true;
               }
            }
            
            GmatBase *obj = FindObject(objName);
            if (obj == NULL)
            {
                std::string errstr = objName;
                errstr += ": Object was not found";
                throw InterpreterException(errstr);
            }
            
            // PropSetup has configuration info for the member objects, so it 
            // gets special treatment
            if ((sar.size() > 1) && (obj->GetType() == Gmat::PROP_SETUP))
            {
                if (!InterpretPropSetupParameter(obj, sar, phrase))
                    throw InterpreterException(
                       "PropSetup Parameter was not recognized");
            }
            else if ((sar.size() > 1) &&
                     (obj->GetType() == Gmat::COORDINATE_SYSTEM))
            {
                if (!InterpretCoordinateSystemParameter(obj, sar, phrase))
                    throw InterpreterException(
                       "Coordinate system parameter was not recognized in \"" +
                       line + "\"");
            }
            else
            {
               try
               {
                   // Handle the case of object = something
                   if (sar.size() == 1)
                   {
                      // Objects can be set to match other objects (copy c'tors 
                      // need to be correct)
                      if (!EquateObjects(obj))
                      {
                         // Arrays and variables can be set directly
                         if (obj->GetTypeName() == "Variable")
                         {
                            if (SetVariable(obj))
                            {
                               // Variable set successfully
                               chunks.clear();
                               return true;
                            }
                         }
                         else if (obj->GetTypeName() == "Array")
                         {
                            #ifdef DEBUG_ARRAY_ASSIGNMENT
                               MessageInterface::ShowMessage(
                                  "Building array assignment for \"" +
                                  line + "\"\n");
                               MessageInterface::ShowMessage(
                                  "Here are the substrings:\n");
                               for (std::vector<std::string *>::iterator cstr =
                                    chunks.begin(); cstr != chunks.end(); ++cstr)
                                  MessageInterface::ShowMessage("   %s\n",
                                     (*cstr)->c_str());
                            #endif

                            if (SetArray(obj))
                            {
                               // Array set successfully
                               chunks.clear();
                               return true;
                            }

                            chunks.clear();
                            throw InterpreterException(
                               "Failure while attempting to set an array element "
                               " on line \"" + line + "\"");
                         }
                         else
                            chunks.clear();
                            throw InterpreterException(
                               "Attempting to set an object, type " +
                               obj->GetTypeName() + ", in an unknown context;" +
                               " see the line\n   \"" +
                               line + "\"");
                      }
                      // Objects equated
                      chunks.clear();
                      return true;
                   }
                                 
                   // Set object associations
                   std::string objParm = sar[1];
                   
                   Integer id = obj->GetParameterID(objParm);
                   
                   /// @todo Correct epoch handling the Spacecraft code
                   // Code to handle "Sat.Epoch.TAIGregorian", etc.
                   if ((objParm == "Epoch") && 
                       (obj->GetType() == Gmat::SPACECRAFT)) {
                       if (sar.size() > 2) {
                         // obj->SetStringParameter("DateFormat", sar[2]);
                         ((Spacecraft*)obj)->SetDisplayDateFormat(sar[2].c_str());
                         unsigned int start, end;
                         start = line.find("=") + 1;
                         const char* linestr = line.c_str();
                         while (linestr[start] == ' ')
                            ++start;
                         end = line.find(";");
                         if (end == std::string::npos)
                            end = line.length()-1;
                         while (linestr[end] == ' ')
                            --end;
   
                         std::string epstr;
                         epstr.assign(line, start, end-start);
                         
                         ((Spacecraft*)obj)->SetDisplayEpoch(epstr);
                         // Force the epoch into the spacecraft data
                         ((Spacecraft*)obj)->SaveDisplay();
                         chunks.clear();
                         return true;
                      }
                      else {
                         // Treat it as a ModJulian format epoch
                         unsigned int start = line.find("=") + 1, stop;
                         const char* linestr = line.c_str();
                         std::string displayEp;
                         while (linestr[start] == ' ')
                            ++start;
                         stop = start;
                         while ((linestr[stop] != ' ') &&
                                (linestr[stop] != '%') &&
                                (linestr[stop] != ';') &&
                                (stop < strlen(linestr)))
                            ++stop;
                         displayEp = line.substr(start, stop-start);
                         ((Spacecraft*)obj)->SetDisplayDateFormat("TAIModJulian");
                         ((Spacecraft*)obj)->SetDisplayEpoch(displayEp.c_str());
                         
                         // Force the epoch into the spacecraft data
                         ((Spacecraft*)obj)->SaveDisplay();
                         // obj->SetRealParameter(objParm, &(linestr[start]));
                      }
                   }
                   
                   // Look for owned objects if the list is deeper than 2
                   if (sar.size() > 2) {
//                      // Check to see if it's a subparameter first
//                      if (obj->GetParameterType(id) == Gmat::STRING_TYPE) {
//                         obj->SetStringParameter(objParm, sar[2]);
//
//                      }
//                      
//                      if (obj->GetParameterType(id) == Gmat::OBJECT_TYPE) {
                         // Maybe it's an owned object
                         obj = FindOwnedObject(sar, obj, 1);
                         if (obj == NULL) {
                            std::string errstr = objName;
                            errstr += sar[1];
                            errstr += ": Object was not found";
                            throw InterpreterException(errstr);
                         }
                         objParm = sar[sar.size() - 1];
                         id = obj->GetParameterID(objParm);
//                      }
                   }
       
                   // Set parameter data
                   
                   ++phrase;
                   if (**phrase == "=")
                       ++phrase;

                   #ifdef DEBUG_PARAMETER_PARSING
                      std::cout << "Setting \"" << (**phrase) 
                                << "\" on object \""<< obj->GetName() << "\"\n";
                   #endif
                   
                   if (!ConstructRHS(obj, **phrase, objParm)) {
      
                      StringArray sa;
                      if (IsGroup((**phrase).c_str()))
                          sa = Decompose(**phrase);
                      else
                          sa.push_back(**phrase);
                      for (StringArray::iterator i = sa.begin(); i != sa.end(); ++i) {
                         #ifdef DEBUG_PARAMETER_PARSING
                            std::cout << "Calling SetParameter with \"" << *i 
                                      << "\" on object \""<< obj->GetName() << "\"\n";
                         #endif
                         if (!SetParameter(obj, id, *i)) {
                            if (obj->GetType() == Gmat::FORCE_MODEL)
                               ConfigureForce((ForceModel*)(obj), objParm, *i);
                         }
                      }
                   }
                }
                catch (BaseException &ex)
                {
                   throw;
                }
            }
        }
        // Then check to see if it's a command
        else if (find(cmdmap.begin(), cmdmap.end(), **phrase) != cmdmap.end())
        {
           if (**phrase == "GMAT")
           {
               #ifdef DEBUG_GMAT_LINE
                  MessageInterface::ShowMessage(
                      "Sequence started; Decomposing GMAT line\n\"%s\"\n",
                      line.c_str());
                  for (std::vector<std::string *>::iterator c = chunks.begin();
                       c != chunks.end(); ++c)
                     MessageInterface::ShowMessage("   \"%s\"\n", (*c)->c_str());
               #endif

               // Look up related object(s)
               ++phrase;

               bool hasEquals = (line.find("=") != std::string::npos ? true : false);
               bool isSinglet = true;
               
               if (SeparateDots(**phrase).size() != 1)
                  isSinglet = false;
               
               // Check to see if this is a function return
               if (((**phrase)[0] == '[') || (!hasEquals && isSinglet))
               {
                  // It's one or more return parameters, so line is a function call
                  #ifdef DEBUG_GMAT_LINE
                     MessageInterface::ShowMessage("This line is a function interface\n",
                                                line.c_str());
                  #endif
                  
                  return InterpretFunctionCall();
               }
               
               if (isSinglet)
               {
                  // hasEquals == true, is a singlet, so a variable or object
                  GmatBase *obj = FindObject(**phrase);


                  if (!EquateObjects(obj))
                  {
                     // Arrays and variables can be set directly
                     if (obj->GetTypeName() == "Variable")
                     {
                        GmatCommand *cmd = moderator->AppendCommand("GMAT", "");
                        cmd->SetGeneratingString(line);
                        if (SetVariable(obj, "", cmd))
                        {
                           // Variable set successfully
                           chunks.clear();
                           branchDepth += cmd->DepthIncrement();
                           return true;
                        }
                     }
                     else if (obj->GetTypeName() == "Array")
                     {
                        #ifdef DEBUG_ARRAY_ASSIGNMENT
                           MessageInterface::ShowMessage(
                              "Building array assignment for \"" +
                              line + "\"\n");
                           MessageInterface::ShowMessage(
                              "Here are the substrings:\n");
                           for (std::vector<std::string *>::iterator cstr =
                                chunks.begin(); cstr != chunks.end(); ++cstr)
                              MessageInterface::ShowMessage("   %s\n",
                                 (*cstr)->c_str());
                        #endif
                        GmatCommand *cmd = moderator->AppendCommand("GMAT", "");
                        cmd->SetGeneratingString(line);
                        if (SetArray(obj, cmd))
                        {
                           // Array set successfully
                           chunks.clear();
                           return true;
                        }

                        chunks.clear();
                        throw InterpreterException(
                           "Failure while attempting to set an array element "
                           " on line \"" + line + "\"");
                     }
                     else
                     {
                        chunks.clear();
                        throw InterpreterException(
                           "Attempting to set an object, " + obj->GetName() +
                           ", in an unknown context; see the line\n   \"" +
                           line + "\"");
                     }
                  }
               }
               // Reset phrase and continue
               phrase = chunks.begin();
            }

            GmatCommand *cmd = moderator->AppendCommand(**phrase, "");
            try
            {
               cmd->SetGeneratingString(line);
               // Temporarily continue to support InterpretAction until all 
               // commands are moved to the new format
               if (!cmd->InterpretAction()) {
                  if (!AssembleCommand(line, cmd))
                     throw InterpreterException("Could not construct command \"" +
                                                line + "\"");
               }
               sequenceStarted = true;
               branchDepth += cmd->DepthIncrement();
            }
            catch (BaseException &e)
            {
                chunks.clear();
                throw;
            }
        }
        // Looks like the line was not understood
        else {
           MessageInterface::ShowMessage
              ("ScriptInterpreter::Parse() cannot interpret the line\n   \"%s\"",
              line.c_str());
           chunks.clear();
           return false;
        }

        // Clear the array of words found in the line
        chunks.clear();
    }
    
    return true;
}


//------------------------------------------------------------------------------
// bool WriteScript()
//------------------------------------------------------------------------------
/**
 * Writes a script -- including all configured objects -- to the output stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::WriteScript()
{
   *outstream << "% GMAT Script File\n% GMAT Release " << REV_STRING << "\n\n";
     
   // First write out the objects, one type at a time
   StringArray::iterator current;
   StringArray objs;
    
   // Hardware
   objs = moderator->GetListOfConfiguredItems(Gmat::HARDWARE);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING 
      std::cout << "Found " << objs.size() << " hardware Components\n";
   #endif

   // Write out tanks first, then thrusters   
   GmatBase *object;
   for (current = objs.begin(); current != objs.end(); ++current) {
      object = FindObject(*current);
      if (object->GetTypeName() == "FuelTank")
         if (!BuildObject(*current))
            return false;
   }
   for (current = objs.begin(); current != objs.end(); ++current) {
      object = FindObject(*current);
      if (object->GetTypeName() == "Thruster")
         if (!BuildObject(*current))
            return false;
   }
            
   // Spacecraft
   objs = moderator->GetListOfConfiguredItems(Gmat::SPACECRAFT);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Spacecraft\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;

   // Formations
   objs = moderator->GetListOfConfiguredItems(Gmat::FORMATION);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Spacecraft\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;

   // Force Models
   objs = moderator->GetListOfConfiguredItems(Gmat::FORCE_MODEL);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Force Models\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;
            
   // Propagator setups
   objs = moderator->GetListOfConfiguredItems(Gmat::PROP_SETUP);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Propagators\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;
            
   // Burn objects
   objs = moderator->GetListOfConfiguredItems(Gmat::BURN);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Burns\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;
    
   // Solver objects
   objs = moderator->GetListOfConfiguredItems(Gmat::SOLVER);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Solvers\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;
    
   // Subscriber setups
   objs = moderator->GetListOfConfiguredItems(Gmat::SUBSCRIBER);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Subscribers\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;

   // Array and Variable setups
   objs = moderator->GetListOfConfiguredItems(Gmat::PARAMETER);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Parameters\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      if (!BuildUserObject(*current))
         return false;
   }

   // Coordinate System setups
   objs = moderator->GetListOfConfiguredItems(Gmat::COORDINATE_SYSTEM);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Coordinate Systems\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      if (!BuildObject(*current))
         return false;
   }

   // Function setups
   objs = moderator->GetListOfConfiguredItems(Gmat::FUNCTION);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Functions\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;


   // Command sequence
   GmatCommand *cmd = moderator->GetNextCommand();
   bool inTextMode = false;
   while (cmd != NULL) {
      if (!inTextMode)
         *outstream << (cmd->GetGeneratingString()) << "\n";
         
      if (cmd->GetTypeName() == "BeginScript")
         inTextMode = true;
      if (cmd->GetTypeName() == "EndScript")
         inTextMode = false;
         
      if (cmd == cmd->GetNext())
         throw InterpreterException("Self-reference found in command stream during write.");
      cmd = cmd->GetNext();
   }
    
   return true;
}


//------------------------------------------------------------------------------
// bool ConfigureCommand(GmatCommand *)
//------------------------------------------------------------------------------
/**
 * Configures GMAT commands.
 * 
 * @return true if the file parses successfully, false on failure.
 * 
 * @note This method is not implemented.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ConfigureCommand(GmatCommand *)
{
    return false;
}


//------------------------------------------------------------------------------
// bool ConfigureMathematics()
//------------------------------------------------------------------------------
/**
 * Configures mathematics embedded in a script.
 * 
 * @return true if the file parses successfully, false on failure.
 * 
 * @note This method is not implemented (Build 5?).
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ConfigureMathematics()
{
    return false;
}

