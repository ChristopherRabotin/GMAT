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
#include "Moderator.hpp"         // class's header file
#include "GmatCommand.hpp"
#include "Assignment.hpp"
#include "StringUtil.hpp"        // for ToDouble()
#include "CommandUtil.hpp"       // for GetCommandScript()
#include <fstream>

// Maybe put something like this in the Gmat namespace?
#define REV_STRING "Build Date: " __DATE__ " at " __TIME__


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
   : Interpreter()
{
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
bool ScriptInterpreter::Build(Gmat::WriteMode mode)
{
    if (!initialized)
        Initialize();
    
    return WriteScript(mode);
}


//------------------------------------------------------------------------------
// bool Build(const std::string &scriptfile, Gmat::WriteMode mode)
//------------------------------------------------------------------------------
/**
 * Writes the currently configured data to a file.
 * 
 * @param scriptfile Name of the output file.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Build(const std::string &scriptfile, Gmat::WriteMode mode)
{
    bool retval = false;
    
    filename = scriptfile;

    std::ofstream outFile(scriptfile.c_str());
    outstream = &outFile;

    retval = Build(mode);
    
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
   
   theModerator->SetCommandsUnchanged();
   
   if (branchDepth != 0) 
   {
      // Clear the command sequence
      theModerator->ClearCommandSeq();
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
    try
    {
       ChunkLine();
    }
    catch (BaseException &e)
    {
       MessageInterface::ShowMessage(
          "The line '%s' failed to parse; the error is \n   '%s'", line.c_str(),
          e.GetMessage().c_str());
       
       return false;
    }
    
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
            std::string arrayName;
            Integer arrayPart = 0;
            
            while (phrase != chunks.end())
            {
                name = **phrase;
                
                if ((name == ";") || (name[0] == '%'))
                   break;

                if (name[0] == '(')
                {
                   throw InterpreterException(
                      "Unknown characters found in Create line.\nPlease check " +
                      name + " in the line:\n" + line + "\n" + 
                      "(Perhaps you meant square braces to size an array?)\n");
                }

                // if type is Array append next phrase, the first part is array name,
                // the second part is array dimension
                if (type == "Array")
                {
                   arrayPart = arrayPart + 1;

                   if (arrayPart == 1)
                   {
                      arrayName = name;
                      ++phrase;
                      continue;
                   }
                   else if (arrayPart == 2)
                   {
                      name = arrayName + name;
                      arrayPart = 0;
                   }
                }
                
                
                if (!InterpretObject(type, name))
                   throw InterpreterException("Unable to create object " +
                                              name + " of type " + type +
                                              "\nScript text: \"" + line +
                                              "\"\n");
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
                       "PropSetup Parameter was not recognized\n");
            }
            else if ((sar.size() > 1) &&
                     (obj->GetType() == Gmat::COORDINATE_SYSTEM))
            {
                if (!InterpretCoordinateSystemParameter(obj, sar, phrase))
                    throw InterpreterException(
                       "Coordinate system parameter was not recognized in \"" +
                       line + "\"\n");
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
                               " on line \"" + line + "\"\n");
                         }
                         else
                         {
                            chunks.clear();
                            throw InterpreterException(
                               "Attempting to set an object, type " +
                               obj->GetTypeName() + ", in an unknown context;" +
                               " see the line\n   \"" +
                               line + "\"\n");
                         }
                      } // if (!EquateObjects(obj))
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
                       (obj->GetType() == Gmat::SPACECRAFT)) 
                   {
                       if (sar.size() > 2) 
                       {
                          // obj->SetStringParameter("DateFormat", sar[2]);
                         ((Spacecraft*)obj)->SetDateFormat(sar[2].c_str());
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
                         ((Spacecraft*)obj)->SetEpoch(epstr);
                         // Force the epoch into the spacecraft data
//                         ((Spacecraft*)obj)->SaveDisplay();
                         chunks.clear();
                         return true;
                      }
                      else 
                      {
                         // Treat it as an epoch formated in current DateFormat
                         unsigned int start = line.find("=") + 1, stop;
                         const char* linestr = line.c_str();
                         std::string displayEp;
                         while (linestr[start] == ' ')
                            ++start;
                         stop = start;
                         while (/*(linestr[stop] != ' ') &&*/
                                (linestr[stop] != '%') &&
                                (linestr[stop] != ';') &&
                                (stop < strlen(linestr)))
                            ++stop;
                         std::string epstr = line.substr(start, stop-start);
                         displayEp = epstr;
                         ((Spacecraft*)obj)->SetEpoch(epstr);
                         
                         chunks.clear();
                         return true;
                      }
                   }  // Completes epoch parse
                   
                   // Special anomaly handling.  Sigh.
                   if ((obj->GetType() == Gmat::SPACECRAFT) && 
                       (id == obj->GetParameterID("TA")))
                   {
                      if ((objParm == "TA") || (objParm == "MA") || 
                          (objParm == "EA"))
                         obj->SetStringParameter("AnomalyType", objParm);
                   }
                   
                   // Look for owned objects if the list is deeper than 2
                   if (sar.size() > 2) 
                   {
//                      // Check to see if it's a subparameter first
//                      if (obj->GetParameterType(id) == Gmat::STRING_TYPE) {
//                         obj->SetStringParameter(objParm, sar[2]);
//
//                      }
//                      
//                      if (obj->GetParameterType(id) == Gmat::OBJECT_TYPE) {
                         // Maybe it's an owned object
                         obj = FindOwnedObject(sar, obj, 1);
                         if (obj == NULL) 
                         {
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
                   std::string objPhrase;
                   if (**phrase == "=")
                   {
                       ++phrase;
                       if (phrase == chunks.end())
                          objPhrase = "";
                       else
                          objPhrase = (**phrase);
                   }

                   #ifdef DEBUG_PARAMETER_PARSING
                      std::cout << "Setting \"" << (**phrase) 
                                << "\" on object \""<< obj->GetName() << "\"\n";
                   #endif
                   
                   if (!ConstructRHS(obj, objPhrase, objParm)) {
                      StringArray sa;
                      if (phrase == chunks.end())
                         sa.push_back("");
                      else
                      {
                         if (IsGroup((**phrase).c_str()))
                            sa = Decompose(**phrase);
                         else
                            sa.push_back(**phrase);
                      }
                      for (StringArray::iterator i = sa.begin(); i != sa.end(); ++i) {
                         #ifdef DEBUG_PARAMETER_PARSING
                            std::cout << "Calling SetParameter with \"" << *i 
                                      << "\" on object \""<< obj->GetName() << "\"\n";
                            MessageInterface::ShowMessage
                               ("Calling SetParameter with %s on object %s\n", (*i).c_str(),
                                obj->GetName().c_str());
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
        } //else if ((**phrase == "GMAT") && (!sequenceStarted))
        
        // Then check to see if it's a command
        else if (find(commandList.begin(), commandList.end(), **phrase) != commandList.end())
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
                  // Test if RHS object exist (loj: 3/17/06)
                  if (obj == NULL)
                     throw InterpreterException(
                        "The object: " + **phrase + " was not defined " +
                        "on line \"" + line + "\"\n");

                  if (!EquateObjects(obj))
                  {
                     // Arrays and variables can be set directly
                     if (obj->GetTypeName() == "Variable")
                     {
                        GmatCommand *cmd = theModerator->AppendCommand("GMAT", "");
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
                        //MessageInterface::ShowMessage("=====> isSinglet = true\n");
                                                      
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
                           
                        GmatCommand *cmd = theModerator->AppendCommand("GMAT", "");
                        cmd->SetGeneratingString(line);

                        if (SetArray(obj, cmd))
                        {
//                            // if RHS is not a equation, check for undefined object
//                            if (((Assignment*)cmd)->GetMathTree() == NULL)
//                            {
//                               //MessageInterface::ShowMessage
//                               //   ("==> obj=%s, RHS=%s\n", obj->GetName().c_str(),
//                               //    (*chunks.back()).c_str());
//                               Real rval = -9999.999;
                              
//                               // If RHS is not a number, find the object
//                               if (!GmatStringUtil::ToDouble(*chunks.back(), &rval))
//                               {
//                                  GmatBase *obj = FindObject(*chunks.back());
//                                  if (obj == NULL)
//                                  {
//                                     std::string str = *chunks.back();
                                    // Check for ' for matrix transpose and ^(-1) for inverse
//                                     if ((str.find("'") == str.npos) &&
//                                         (str.find("^(-1)") == str.npos))
//                                     {
//                                        throw InterpreterException
//                                           ("The object: " + *chunks.back() + " was not defined " +
//                                            "on line \"" + line + "\"\n");
//                                     }
//                                  }
//                               }
//                            }
                           
                           // Array set successfully
                           chunks.clear();
                           return true;
                        }

                        chunks.clear();
                        throw InterpreterException(
                           "Failure while attempting to set an array element "
                           " on line \"" + line + "\"\n");
                     }
                     else
                     {
                        chunks.clear();
                        throw InterpreterException(
                           "Attempting to set an object, " + obj->GetName() +
                           ", in an unknown context; see the line\n   \"\n" +
                           line + "\"");
                     }
                  }
               }
               // Reset phrase and continue
               phrase = chunks.begin();
            }
            
            #ifdef DEBUG_COMMAND_PARSING
               MessageInterface::ShowMessage("Line '%s' is a command\n", 
                  line.c_str());
            #endif
            GmatCommand *cmd = theModerator->AppendCommand(**phrase, "");
            try
            {
               cmd->SetGeneratingString(line);
               // Temporarily continue to support InterpretAction until all 
               // commands are moved to the new format
               
               //MessageInterface::ShowMessage
               //   ("=====> Parse() calling InterpretAction() line=%s\n", line.c_str());
               
               if (!cmd->InterpretAction()) 
               {
                  //MessageInterface::ShowMessage
                  //   ("=====> Parse() calling AssembleCommand()\n");
                  
                  if (!AssembleCommand(line, cmd))
                  {
                     throw InterpreterException(
                        "Could not construct command \"" + line + "\"\n");
                  }
               }
               sequenceStarted = true;
               branchDepth += cmd->DepthIncrement();
            }
            catch (BaseException &e)
            {
               #ifdef DEBUG_COMMAND_PARSING
                  MessageInterface::ShowMessage(
                     "Caught an exception '%s' from command of type '%s'\n",
                     e.GetMessage().c_str(), cmd->GetTypeName().c_str());
               #endif
                                 
               /// Clean up the resulting mess
               chunks.clear();
               
               throw InterpreterException("The command line '" + line + 
                  "' did not parse correctly; it threw the message \n   '"
                  + e.GetMessage() +"'\n");
            }
        }
        // Looks like the line was not understood
        else {
           MessageInterface::ShowMessage
              ("ScriptInterpreter::Parse() cannot interpret the line\n   \"%s\"\n"
               "May there be missing GMAT keyword?",
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
// bool WriteScript(Gmat::WriteMode mode = Gmat::SCRIPTING)
//------------------------------------------------------------------------------
/**
 * Writes a script -- including all configured objects -- to the output stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::WriteScript(Gmat::WriteMode mode)
{
   if (mode == Gmat::EPHEM_HEADER)
   {
      *outstream << "% GMAT Ephemeris File\n";
      *outstream << "% GMAT Build Date: " <<  __DATE__ << "\n\n";
   }
   else
   {
      // Here add test for header comment; if it does not exist, use this
      *outstream << "%% GMAT Script File\n%% " << REV_STRING << "\n\n";
   }
   
   // First write out the objects, one type at a time
   StringArray::iterator current;
   StringArray objs;
   
   // Hardware
   objs = theModerator->GetListOfObjects(Gmat::HARDWARE);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING 
      std::cout << "Found " << objs.size() << " hardware Components\n";
   #endif

   // Write out tanks first, then thrusters   
   GmatBase *object;
   for (current = objs.begin(); current != objs.end(); ++current) {
      object = FindObject(*current);
      if (object->GetTypeName() == "FuelTank")
         if (!BuildObject(*current, mode))
            return false;
   }
   for (current = objs.begin(); current != objs.end(); ++current) {
      object = FindObject(*current);
      if (object->GetTypeName() == "Thruster")
         if (!BuildObject(*current, mode))
            return false;
   }
            
   // Spacecraft
   objs = theModerator->GetListOfObjects(Gmat::SPACECRAFT);

   // Setup the coordinate systems on Spacecraft so they can perform conversions
   CoordinateSystem *ics = theModerator->GetInternalCoordinateSystem(), *sccs;

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Spacecraft\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
   {
      Spacecraft *sc = (Spacecraft*)(theModerator->GetObject(*current));
      sc->SetInternalCoordSystem(ics);
      sccs = (CoordinateSystem*)(theModerator->
         GetObject(sc->GetRefObjectName(Gmat::COORDINATE_SYSTEM)));
      if (sccs)
         sc->SetRefObject(sccs, Gmat::COORDINATE_SYSTEM);
      sc->Initialize();
      
      if (!BuildObject(*current, mode))
         return false;
   }
   
   // Formations
   objs = theModerator->GetListOfObjects(Gmat::FORMATION);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Spacecraft\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current, mode))
         return false;

   // Libration Points and Barycenters
   objs = theModerator->GetListOfObjects(Gmat::CALCULATED_POINT);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      MessageInterface::ShowMessage("Found %d calculated points\n", objs.size());
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current, mode))
         return false;
            
   // Force Models
   objs = theModerator->GetListOfObjects(Gmat::FORCE_MODEL);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Force Models\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current, mode))
         return false;
   
   // Propagator setups
   objs = theModerator->GetListOfObjects(Gmat::PROP_SETUP);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Propagators\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current, mode))
         return false;
   
   // Burn objects
   objs = theModerator->GetListOfObjects(Gmat::BURN);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Burns\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current, mode))
         return false;
    
   // Solver objects
   objs = theModerator->GetListOfObjects(Gmat::SOLVER);
   #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      std::cout << "Found " << objs.size() << " Solvers\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current, mode))
         return false;
   
   // If mode is EPHEM_HEADER, skip this part
   // Subscriber setups
   if (mode != Gmat::EPHEM_HEADER)
   {
      objs = theModerator->GetListOfObjects(Gmat::SUBSCRIBER);
      #ifdef DEBUG_SCRIPT_READING_AND_WRITING
         std::cout << "Found " << objs.size() << " Subscribers\n";
      #endif
      for (current = objs.begin(); current != objs.end(); ++current)
      {
         // loj: 4/6/06 Skip TextEphemFile for now, because it is under Tools
         GmatBase *obj = FindObject(*current);
         if (obj != NULL && obj->GetTypeName() != "TextEphemFile")
            if (!BuildObject(*current, mode))
               return false;
      }
      
      // Array and Variable setups
      objs = theModerator->GetListOfObjects(Gmat::PARAMETER);
      #ifdef DEBUG_SCRIPT_READING_AND_WRITING
         std::cout << "Found " << objs.size() << " Parameters\n";
      #endif
      for (current = objs.begin(); current != objs.end(); ++current)
      {
         if (!BuildUserObject(*current, mode))
            return false;
      }
   
      // Coordinate System setups
      objs = theModerator->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
      #ifdef DEBUG_SCRIPT_READING_AND_WRITING
         std::cout << "Found " << objs.size() << " Coordinate Systems\n";
      #endif
      for (current = objs.begin(); current != objs.end(); ++current)
      {
         if (!BuildObject(*current, mode))
            return false;
      }

      // Function setups
      objs = theModerator->GetListOfObjects(Gmat::FUNCTION);
      #ifdef DEBUG_SCRIPT_READING_AND_WRITING
         std::cout << "Found " << objs.size() << " Functions\n";
      #endif
      for (current = objs.begin(); current != objs.end(); ++current)
         if (!BuildObject(*current, mode))
            return false;

   }
   
   // Command sequence
   GmatCommand *cmd = theModerator->GetNextCommand();
   bool inTextMode = false;
   
   while (cmd != NULL) {
      
      #ifdef DEBUG_SCRIPT_READING_AND_WRITING
      MessageInterface::ShowMessage
         ("===> ScriptInterpreter::WriteScript() before write cmd=%s, mode=%d, "
          "inTextMode=%d\n", cmd->GetTypeName().c_str(), mode, inTextMode);
      #endif
      
      if (!inTextMode)
         *outstream << (cmd->GetGeneratingString()) << "\n";
         
      if (cmd->GetTypeName() == "BeginScript")
         inTextMode = true;
      if (cmd->GetTypeName() == "EndScript")
         inTextMode = false;
      
      if (cmd == cmd->GetNext())
         throw InterpreterException("Self-reference found in command stream during write.\n");
      
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

