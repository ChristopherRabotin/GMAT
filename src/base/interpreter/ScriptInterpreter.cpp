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
#define REV_STRING "Build 3, July 2004"


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
    StringArray::iterator current = cmdmap.begin(), last = cmdmap.end();
    while (current != last) {
       ++current;
    }
    
    if (instream->fail() || instream->eof()) {
        return false;
    }
    
    while (!instream->eof()) {
        if (!ReadLine())
            return false;
        if (!Parse())
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

//*
        if ((**phrase == "GMAT") && (!sequenceStarted)) {
            // Look up related object(s)
            ++phrase;
            std::string objName = GetToken(**phrase);
            GmatBase *obj = FindObject(objName);
            if (obj == NULL) {
                std::string errstr = objName;
                errstr += ": Object was not found";
                throw InterpreterException(errstr);
            }
            
            // PropSetup has configuration info for the member objects, so it 
            // gets special treatment
            if (obj->GetType() == Gmat::PROP_SETUP) {
                if (!InterpretPropSetupParameter(obj, phrase))
                    throw InterpreterException("PropSetup Parameter was not recognized");
            }
            else 
            {
                // Set object associations
                std::string objParm = GetToken();
                Integer id = obj->GetParameterID(objParm);
    
                if (obj->GetParameterType(id) == Gmat::UNKNOWN_PARAMETER_TYPE)
                {
                    // Could be a member object -- check that first
                    
                    /// @todo Fill in the parsing for multipart strings
                    std::string subparm = GetToken();
                    if (subparm == "")
                        throw InterpreterException("Assignment string does not parse: " + objParm); //loj: added objParm
                    // Find the owned object
                    // Set the parm on the owned object
                }
                else
                {
                    // Set parameter data
                    ++phrase;
                    if (**phrase == "=")
                        ++phrase;
                    SetParameter(obj, id, **phrase);
                }
            }
        }
//*/
        // Check to see if it's a command
        else if (find(cmdmap.begin(), cmdmap.end(), **phrase) != cmdmap.end()) {
            GmatCommand *cmd = moderator->AppendCommand(**phrase, "");
            cmd->SetGeneratingString(line);
            
            //loj: 3/24/04 added try block
            try
            {
                cmd->InterpretAction();
                sequenceStarted = true;
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
    
    // Spacecraft
    objs = moderator->GetListOfConfiguredItems(Gmat::SPACECRAFT);
    for (current = objs.begin(); current != objs.end(); ++current)
        if (!BuildObject(*current))
            return false;
            
    // Force Models
    objs = moderator->GetListOfConfiguredItems(Gmat::FORCE_MODEL);
    for (current = objs.begin(); current != objs.end(); ++current)
        if (!BuildObject(*current))
            return false;
            
    // Propagator setups
    objs = moderator->GetListOfConfiguredItems(Gmat::PROP_SETUP);
    for (current = objs.begin(); current != objs.end(); ++current)
        if (!BuildObject(*current))
            return false;
            
    // Burn objects
    objs = moderator->GetListOfConfiguredItems(Gmat::BURN);
    for (current = objs.begin(); current != objs.end(); ++current)
        if (!BuildObject(*current))
            return false;
    
    // Solver objects
    objs = moderator->GetListOfConfiguredItems(Gmat::SOLVER);
    for (current = objs.begin(); current != objs.end(); ++current)
        if (!BuildObject(*current))
            return false;
    
    // Subscriber setups
    objs = moderator->GetListOfConfiguredItems(Gmat::SUBSCRIBER);
    for (current = objs.begin(); current != objs.end(); ++current)
        if (!BuildObject(*current))
            return false;

    // Command sequence
    GmatCommand *cmd = moderator->GetNextCommand();
    while (cmd != NULL) {
        *outstream << (cmd->GetGeneratingString()) << "\n";
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

