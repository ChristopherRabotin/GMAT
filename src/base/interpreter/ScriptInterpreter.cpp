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
#define REV_STRING "Build 3.2, October 2004"


//#define DEBUG_SCRIPTINTERPRETER
//#define DEBUG_SCRIPT_READING_AND_WRITING 
//#define DEBUG_PARAMETER_PARSING

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
bool ScriptInterpreter::Interpret(void)
{
    if (!initialized)
        Initialize();
        
    sequenceStarted = false;
    return ReadScript();
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
// bool Build(void)
//------------------------------------------------------------------------------
/**
 * Writes the currently configured data to an output stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Build(void)
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
// bool ReadScript(void)
//------------------------------------------------------------------------------
/**
 * Reads a script from the input stream line by line and parses it.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ReadScript(void)
{
    branchDepth = 0;
    
    if (instream->fail() || instream->eof()) {
        return false;
    }
    
    chunks.clear();
    
    while (!instream->eof()) {
        if (!ReadLine())
            return false;
        if (!Parse())
            return false;
    }
    
    if (branchDepth != 0) {
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
// bool ReadLine(void)
//------------------------------------------------------------------------------
/**
 * Reads a line from the input stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ReadLine(void)
{
    char charLine[4096] = "";
    
    instream->getline(charLine, 4095);
    line = charLine;
//    MessageInterface::ShowMessage("ScriptInterpreter::ReadLine \""
//                                  + line + "\"\n");
    return true;
}


//------------------------------------------------------------------------------
// bool Parse(void)
//------------------------------------------------------------------------------
/**
 * Builds or configures GMAT objects based on the current line of script.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::Parse(void)
{
    // Determine what kind of line we have
    ChunkLine();
    
    // Process accordingly
    if (!chunks.empty())
    {
        std::vector<std::string*>::iterator phrase = chunks.begin();

        if (**phrase == "Create") {
            // Instantiate the core object
            std::string type, name = "";
            ++phrase;
            type = **phrase;
            ++phrase;
            if (phrase != chunks.end())
                name = **phrase;

            if (!InterpretObject(type, name))
                throw InterpreterException("Unable to create object: " + name); //loj: added name
        }


        if ((**phrase == "GMAT") && (!sequenceStarted)) {
            // Look up related object(s)
            ++phrase;

            StringArray sar = SeparateDots(**phrase);
            std::string objName = sar[0];
            GmatBase *obj = FindObject(objName);
            if (obj == NULL) {
                std::string errstr = objName;
                errstr += ": Object was not found";
                throw InterpreterException(errstr);
            }
            
            // PropSetup has configuration info for the member objects, so it 
            // gets special treatment
            if (obj->GetType() == Gmat::PROP_SETUP) {
                if (!InterpretPropSetupParameter(obj, sar, phrase))
                    throw InterpreterException("PropSetup Parameter was not recognized");
            }
            else 
            {
               try {
                   // Set object associations
                   std::string objParm = sar[1];
                   
                   Integer id = obj->GetParameterID(objParm);
                   
                   /// @todo Correct this kludge in the Spacecraft code!!!
                   // This code is a real hack put in place because the 
                   // spacecraft code does NOT conform to the agreed upon 
                   // interfaces for parameter names in GMAT.
                   //
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
                catch (BaseException &ex) {
                   throw;
                }
            }
        }

        // Check to see if it's a command
        else if (find(cmdmap.begin(), cmdmap.end(), **phrase) != cmdmap.end()) {
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

        // Clear the array of words found in the line
        chunks.clear();
    }
    
    return true;
}


//------------------------------------------------------------------------------
// bool WriteScript(void)
//------------------------------------------------------------------------------
/**
 * Writes a script -- including all configured objects -- to the output stream.
 * 
 * @return true if the file parses successfully, false on failure.
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::WriteScript(void)
{
   *outstream << "% GMAT Script File\n% GMAT Release " << REV_STRING << "\n\n";
     
   // First write out the objects, one type at a time
   StringArray::iterator current;
   StringArray objs;
    
   // Hardware
   objs = moderator->GetListOfConfiguredItems(Gmat::HARDWARE);

   #ifdef DEBUG_SCRIPT_READING_AND_WRITING 
      std::cout << "Found " << objs.size() << " Hardware Components\n";
   #endif
   for (current = objs.begin(); current != objs.end(); ++current)
      if (!BuildObject(*current))
         return false;
            
   // Spacecraft
   objs = moderator->GetListOfConfiguredItems(Gmat::SPACECRAFT);

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

   // Command sequence
   GmatCommand *cmd = moderator->GetNextCommand();
   while (cmd != NULL) {
      *outstream << (cmd->GetGeneratingString()) << "\n";
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
// bool ConfigureMathematics(void)
//------------------------------------------------------------------------------
/**
 * Configures mathematics embedded in a script.
 * 
 * @return true if the file parses successfully, false on failure.
 * 
 * @note This method is not implemented (Build 5?).
 */
//------------------------------------------------------------------------------
bool ScriptInterpreter::ConfigureMathematics(void)
{
    return false;
}

