//$Header$
//------------------------------------------------------------------------------
//                                  Interpreter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/08/28
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


//------------------------------------------------------------------------------
// Interpreter()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Interpreter::Interpreter() :
    instream            (NULL),
    outstream           (NULL),
    initialized         (false)
{
    moderator = Moderator::Instance();
    
    if (moderator) {
        // Set up the mapping for the core types
        typemap["Spacecraft"] = Gmat::SPACECRAFT;
        typemap["GroundStation"] = Gmat::GROUND_STATION;
        typemap["Command"] = Gmat::COMMAND;
//        typemap["Propagator"] = Gmat::PROPAGATOR;
        typemap["ForceModel"] = Gmat::FORCE_MODEL;
        typemap["Force"] = Gmat::PHYSICAL_MODEL;
        typemap["SolarSystem"] = Gmat::SOLAR_SYSTEM;
        typemap["CelestialBody"] = Gmat::CELESTIAL_BODY;
        typemap["Parameter"] = Gmat::PARAMETER;
//        typemap["StoppingCondition"] = Gmat::STOP_CONDITION;
        typemap["Subscriber"] = Gmat::SUBSCRIBER;
        typemap["Propagator"] = Gmat::PROP_SETUP;
        typemap["Burn"] = Gmat::BURN;
    }
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
// void Initialize(void)
//------------------------------------------------------------------------------
/**
 * Builds core lists of available objects.
 */
//------------------------------------------------------------------------------
void Interpreter::Initialize(void)
{
    // Build a mapping for all of the defined commands
    StringArray cmds = moderator->GetListOfFactoryItems(Gmat::COMMAND);
    copy(cmds.begin(), cmds.end(), back_inserter(cmdmap));
    
    StringArray props = moderator->GetListOfFactoryItems(Gmat::PROPAGATOR);
    copy(props.begin(), props.end(), back_inserter(propmap));

    StringArray forces = moderator->GetListOfFactoryItems(Gmat::PHYSICAL_MODEL);
    copy(forces.begin(), forces.end(), back_inserter(forcemap));
    
    StringArray subs = moderator->GetListOfFactoryItems(Gmat::SUBSCRIBER);
    copy(subs.begin(), subs.end(), back_inserter(subscribermap));

    StringArray parms = moderator->GetListOfFactoryItems(Gmat::PARAMETER);
    copy(parms.begin(), parms.end(), back_inserter(parametermap));

    StringArray sconds = moderator->GetListOfFactoryItems(Gmat::STOP_CONDITION);
    copy(sconds.begin(), sconds.end(), back_inserter(stopcondmap));

    StringArray svers = moderator->GetListOfFactoryItems(Gmat::SOLVER);
    copy(svers.begin(), svers.end(), back_inserter(solvermap));
    
    #ifdef DEBUG_OBJECT_LISTS
        std::cout << "\nCommands:\n   ";
        for (std::vector<std::string>::iterator c = cmds.begin(); c != cmds.end(); ++c)
            std::cout << *c << "\n   ";

        std::cout << "\nPropagators:\n   ";
        for (std::vector<std::string>::iterator p = props.begin(); p != props.end(); ++p)
            std::cout << *p << "\n   ";

        std::cout << "\nForces:\n   ";
        for (std::vector<std::string>::iterator f = forces.begin(); f != forces.end(); ++f)
            std::cout << *f << "\n   ";

        std::cout << "\nSubscribers:\n   ";
        for (std::vector<std::string>::iterator s = subs.begin(); s != subs.end(); ++s)
            std::cout << *s << "\n   ";

        std::cout << "\nParameters:\n   ";
        for (std::vector<std::string>::iterator pm = parms.begin(); pm != parms.end(); ++pm)
            std::cout << *pm << "\n   ";

        std::cout << "\nStopConds:\n   ";
        for (std::vector<std::string>::iterator sc = sconds.begin(); sc != sconds.end(); ++sc)
           std::cout << *sc << "\n   ";

        std::cout << "\nSolvers:\n   ";
        for (std::vector<std::string>::iterator sc = svers.begin(); sc != svers.end(); ++sc)
           std::cout << *sc << "\n   ";

        std::cout << "\n";
    #endif

    initialized = true;
}


//------------------------------------------------------------------------------
// bool SetInStream(std::istream *str)
//------------------------------------------------------------------------------
/**
 * Defines the input stream that gets interpreted.
 * 
 * @param str The input stream.
 * 
 * @return true on success, false on failure.  (Currently always returns true.)
 */
//------------------------------------------------------------------------------
bool Interpreter::SetInStream(std::istream *str)
{
    instream = str;
    return true;
}


//------------------------------------------------------------------------------
// bool SetOutStream(std::ostream *str)
//------------------------------------------------------------------------------
/**
 * Defines the output stream for writing serialized output.
 * 
 * @param str The output stream.
 * 
 * @return true on success, false on failure.  (Currently always returns true.)
 */
//------------------------------------------------------------------------------
bool Interpreter::SetOutStream(std::ostream *str)
{
    outstream = str;
    return true;
}


//------------------------------------------------------------------------------
// bool Interpret(void)
//------------------------------------------------------------------------------
/**
 * Translates text streams (e.g. scripts and subscripts) into GMAT objects.
 * 
 * This method gets overridden by derived classes to perform the detailed 
 * translation based on the type of stream that is translated.
 * 
 * @return true on success, false on failure.  (Default always returns false.)
 */
//------------------------------------------------------------------------------
bool Interpreter::Interpret(void)
{
    return false;
}


//------------------------------------------------------------------------------
// bool InterpretObject(std::string objecttype, std::string objectname)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to build core objects and place them in the ConfigManager.
 *  
 * @param objecttype Text type for the requested object.
 * @param objectname Name for the object, used for references to the object.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Interpreter::InterpretObject(std::string objecttype, std::string objectname)
{
    if (objecttype == "Spacecraft") {
        CreateSpacecraft(objectname);
        return true;
    }
    
    if (objecttype == "Propagator") {
        // PropSetup *prop =
        CreatePropSetup(objectname);
        // // Add the force model container
        // ForceModel *fm = moderator->CreateForceModel("");
        // prop->SetForceModel(fm);
        return true;
    }

    if (objecttype == "ForceModel") {
        // ForceModel *prop =
        CreateForceModel(objectname);
        return true;
    }
    
    if (objecttype == "ImpulsiveBurn") {
        CreateBurn(objectname, true);
        return true;
    }

    //loj: added
    // Handle Parameters
    if (find(parametermap.begin(), parametermap.end(), objecttype) != 
        parametermap.end())
    {
        moderator->CreateParameter(objecttype, objectname);
        return true;
    }
    
    // Handle Subscribers
    if (find(subscribermap.begin(), subscribermap.end(), objecttype) != 
                                                          subscribermap.end()) {
        moderator->CreateSubscriber(objecttype, objectname);
        return true;
    }

    // Handle Solvers
    if (find(solvermap.begin(), solvermap.end(), objecttype) != 
                                                          solvermap.end()) {
        moderator->CreateSolver(objecttype, objectname);
        return true;
    }

    return false;
}


// The "Build" methods take GMAT objects and serializes the objects (e.g., these
// methods are invoked to write out scripts or subscripts).

//------------------------------------------------------------------------------
// bool BuildObject(std::string &objectname)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to build core objects and place them in the ConfigManager.
 * 
 * @param objectname Name of the object that gets serialized.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Interpreter::BuildObject(std::string &objectname)
{
    GmatBase *obj = FindObject(objectname);
    
    if (obj == NULL) {
        return true; //false;
    }
    
    (*outstream).precision(18);        /// @todo Make output precision generic
    
    // For now, "Create Propagator" creates a PropSetup.  This kludge handles 
    // that special case.
    std::string tname = obj->GetTypeName();
    if (tname == "PropSetup")
        tname = "Propagator";
    *outstream << "Create " << tname << " " 
               << obj->GetName() << "\n";
               
    Integer i;
    for (i = 0; i < obj->GetParameterCount(); ++i) 
    {
        // Skip StringArray parameters, at least for now
        if (obj->GetParameterType(i) != Gmat::STRINGARRAY_TYPE) {
            // Fill in the l.h.s.
            *outstream << "GMAT " << objectname << "." 
                       << obj->GetParameterText(i) << " = ";
            WriteParameterValue(obj, i);
            *outstream << ";\n";
        }
    }
    *outstream << "\n";
    return true;
}


//------------------------------------------------------------------------------
// void WriteParameterValue(GmatBase *obj, Integer id)
//------------------------------------------------------------------------------
/**
 * Writes out parameters in the GMAT script syntax.
 * 
 * @param obj Pointer to the object containing the parameter.
 * @param id  ID for the parameter that gets written.
 */
//------------------------------------------------------------------------------
void Interpreter::WriteParameterValue(GmatBase *obj, Integer id)
{
    Gmat::ParameterType tid = obj->GetParameterType(id);
    
    switch (tid) {
        case Gmat::OBJECT_TYPE:
        case Gmat::STRING_TYPE:     // Strings and objects write out a string
            *outstream << obj->GetStringParameter(id);
            break;
            
        case Gmat::INTEGER_TYPE:
            *outstream << obj->GetIntegerParameter(id);
            break;
            
        case Gmat::REAL_TYPE:
            *outstream << obj->GetRealParameter(id);
            break;
            
        case Gmat::BOOLEAN_TYPE:
            *outstream << ((obj->GetBooleanParameter(id)) ? "true" : "false");
            break;
            
        default:
            break;
    }
}


// The "Create" methods make calls, through the Moderator, to the Factories
// to get new instances of the requested objects

//------------------------------------------------------------------------------
// Spacecraft* CreateSpacecraft(std::string satname)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a new Spacecraft object.
 * 
 * @param satname Name of the spacecraft.
 * 
 * @return Pointer to the constructed Spacecraft.
 */
//------------------------------------------------------------------------------
Spacecraft* Interpreter::CreateSpacecraft(std::string satname)
{
    return moderator->CreateSpacecraft("Spacecraft", satname);
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(std::string commandtype)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a GmatCommand object.
 * 
 * @param commandtype Text descriptor for the command.
 * 
 * @return Pointer to the constructed GmatCommand.
 */
//------------------------------------------------------------------------------
GmatCommand* Interpreter::CreateCommand(std::string commandtype)
{
    return moderator->CreateCommand(commandtype, "");
}


//------------------------------------------------------------------------------
// Propagator* CreatePropagator(std::string proptype)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Propagator object.
 * 
 * @param proptype Text descriptor for the command.
 * 
 * @return Pointer to the constructed Propagator.
 */
//------------------------------------------------------------------------------
Propagator* Interpreter::CreatePropagator(std::string proptype)
{
    return moderator->CreatePropagator(proptype, "");
}


//------------------------------------------------------------------------------
// ForceModel* CreateForceModel(std::string modelname)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a ForceModel object.
 * 
 * @param modelname Name for the ForceModel.
 * 
 * @return Pointer to the constructed ForceModel.
 */
//------------------------------------------------------------------------------
ForceModel* Interpreter::CreateForceModel(std::string modelname)
{
    return moderator->CreateForceModel(modelname);
}


//------------------------------------------------------------------------------
// PhysicalModel* CreatePhysicalModel(std::string forcetype)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create individual Force objects.
 * 
 * @param forcetype The type of force requested.
 * 
 * @return Pointer to the constructed force (aka PhysicalModel).
 */
//------------------------------------------------------------------------------
PhysicalModel* Interpreter::CreatePhysicalModel(std::string forcetype)
{
    return moderator->CreatePhysicalModel(forcetype, "");
}


//------------------------------------------------------------------------------
// SolarSystem* CreateSolarSystem(std::string ssname)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a SolarSystem object.
 * 
 * @param ssname Name for the solar system.
 * 
 * @return Pointer to the constructed SolarSystem.
 */
//------------------------------------------------------------------------------
SolarSystem* Interpreter::CreateSolarSystem(std::string ssname)
{
    return moderator->CreateSolarSystem(ssname);
}


//------------------------------------------------------------------------------
// CelestialBody* CreateCelestialBody(std::string cbname, std::string type)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a CelestialBody.
 * 
 * @param cbname Name for the body.
 * @param type The type of body requested.
 * 
 * @return Pointer to the constructed CelestialBody.
 */
//------------------------------------------------------------------------------
CelestialBody* Interpreter::CreateCelestialBody(std::string cbname, std::string type)
{
    return moderator->CreateCelestialBody(cbname, type);
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(std::string name, std::string type)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Parameter.
 * 
 * @param name Name for the parameter.
 * @param type Type of parameter requested
 * 
 * @return Pointer to the constructed Parameter.
 */
//------------------------------------------------------------------------------
Parameter* Interpreter::CreateParameter(std::string name, std::string type)
{
    return moderator->CreateParameter(type, name);
}


//StoppingCondition* Interpreter::CreateStopCond(std::string conditiontype)
//{
//    return moderator->CreateStopCondition(conditiontype);
//}


//------------------------------------------------------------------------------
// Parameter* CreateSubscriber(std::string name, std::string type)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Subscriber.
 * 
 * @param name Name for the subscriber.
 * @param type Type of parameter requested.
 * 
 * @return Pointer to the constructed Subscriber.
 */
//------------------------------------------------------------------------------
Subscriber* Interpreter::CreateSubscriber(std::string name, std::string type)
{
    return moderator->CreateSubscriber(type, name);
}


                                                
// The following method signature depends on an open scripting issue: if
// props and force models are named, the following Create method should use
// the names rather than the object pointers

//------------------------------------------------------------------------------
// PropSetup* CreatePropSetup(std::string name)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a PropSetup.
 * 
 * @param name Name for the propagator setup.
 * 
 * @return Pointer to the constructed PropSetup.
 */
//------------------------------------------------------------------------------
PropSetup* Interpreter::CreatePropSetup(std::string name)
{
    return moderator->CreatePropSetup(name);
}


//------------------------------------------------------------------------------
// Burn* CreateBurn(std::string name, bool isImpulsive)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a Burn object.
 * 
 * @param name Name for the burn.
 * @param isImpulsive true for impulsive burns, false for finite burns.
 * 
 * @return Pointer to the constructed Burn.
 */
//------------------------------------------------------------------------------
Burn* Interpreter::CreateBurn(std::string name, bool isImpulsive)
{
    if (isImpulsive)
        return moderator->CreateBurn("ImpulsiveBurn", name);
    return NULL; // moderator->CreateBurn("FiniteBurn", name);
}


//------------------------------------------------------------------------------
// bool InterpretPropSetupParameter(GmatBase *obj, 
//                                  std::vector<std::string*>::iterator phrase)
//------------------------------------------------------------------------------
/**
 * Sets PropSetup parameters.
 * 
 * @param obj Object that is being configured.
 * @param phrase Phrase containing the configuration information.
 * 
 * @return true if the parameter is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Interpreter::InterpretPropSetupParameter(GmatBase *obj, 
                                     std::vector<std::string*>::iterator phrase)
{
    bool retval = true;
    
    // Set object associations
    std::string objParm = GetToken();
    Integer id = obj->GetParameterID(objParm);
    
    Gmat::ParameterType parmType = obj->GetParameterType(id);

    if ((parmType != Gmat::UNKNOWN_PARAMETER_TYPE) &&
        (parmType != Gmat::OBJECT_TYPE))
    {
        // Set parameter data
        ++phrase;

        if (**phrase == "=")
            ++phrase;
        SetParameter(obj, id, **phrase);
    }
    else
    {
        if (objParm == "Type") {
            ++phrase;
    
            if (**phrase == "=")
                ++phrase;
            else
                throw InterpreterException("Syntax error creating Propagator");
            Propagator *prop = moderator->CreatePropagator(**phrase, "");
            if (prop)
                ((PropSetup*)obj)->SetPropagator(prop);
            else
                throw InterpreterException("Propagator could not be created");
        }
        else if (objParm == "ForceModelName") {
            ++phrase;
    
            if (**phrase == "=")
                ++phrase;
            else
                throw InterpreterException("Syntax error accessing Force Model");
            ForceModel *fm = moderator->GetForceModel(**phrase);
            if (fm)
                ((PropSetup*)obj)->SetForceModel(fm);
            else
                throw InterpreterException("Force model does not exist");
        }    
        else {
            // Could be a subitem -- Drag.Earth = Exponential, for example
            std::string subparm = GetToken();
            if (subparm == "")
                throw InterpreterException("Assignment string does not parse");
            // Find the owned object
            if (objParm == "Drag") {
                if (subparm != "Earth")
                    throw InterpreterException("Only Earth drag is supported in build 2");
            }
                
            // Set the parm on the owned object
            throw InterpreterException("Assignment string does not parse");
        }
    }
    
    return retval;
}


//------------------------------------------------------------------------------
// void ChunkLine(void)
//------------------------------------------------------------------------------
/**
 * Breaks a line of script into pieces based on white space.
 */
//------------------------------------------------------------------------------
void Interpreter::ChunkLine(void)
{
    // Break the line into pieces based on while space and special characters
    Integer start = 0, end, semicolonLocation = 0;
    const char *str = line.c_str();
    std::string phrase;

    start = SkipWhiteSpace(start); // Find the beginning of the text
    while (start != -1) {
        end = start;
        while ((str[end] != ' ') && (str[end] != '\t') && (str[end] != '\r') &&
               (str[end] != '\n') && (str[end] != '%') && (str[end] != '\0')) {
            ++end;
            if ((str[end] == ';') && (semicolonLocation == 0))
                semicolonLocation = end;
        }
        if (semicolonLocation == 0)
            semicolonLocation = end;
        
        phrase.assign(line, start, (semicolonLocation-start));
        
        chunks.push_back(new std::string(phrase));
        start = SkipWhiteSpace(end);
        semicolonLocation = 0;
    }
}


//------------------------------------------------------------------------------
// Integer SkipWhiteSpace(Integer start)
//------------------------------------------------------------------------------
/**
 * Skips blank spaces and tabs.
 * 
 * @param start Starting point in the script line.
 * 
 * @return Location of the first useful character, or -1 for comments or line
 *         endings.
 */
//------------------------------------------------------------------------------
Integer Interpreter::SkipWhiteSpace(Integer start)
{
    Integer finish = start;
    const char *str = line.c_str();
    if (str[0] == '%')  // Comment line
        return -1;
        
    while ((str[finish] == ' ') || (str[finish] == '\t')) {
        ++finish;
        if ((str[finish] == '\r') || (str[finish] == '\n') ||
            (str[finish] == '%') || (str[finish] == '\0'))
            return -1;
    }
    if (start >= (Integer)strlen(str))
        return -1;
    return finish;
}


//------------------------------------------------------------------------------
// Integer FindDelimiter(std::string str, std::string specChar)
//------------------------------------------------------------------------------
/**
 * Method to find special characters: = , { } [ ] ( ) ; . %
 * 
 * @param str String containing the character.
 * @param specChar The character.
 * 
 * @return Location of the special character, or -1 if the character is not in
 *         the string.
 * 
 * @note This method is not implemented, and always returns -1.
 */
//------------------------------------------------------------------------------
Integer Interpreter::FindDelimiter(std::string str, std::string specChar)
{
    return -1;
}


//------------------------------------------------------------------------------
// std::string GetToken(std::string tokstr)
//------------------------------------------------------------------------------
/**
 * Find the next token in the input string.
 * 
 * @param tokstr String that needs to be broken into tokens.  Pass in the empty
 *               string ("") to continue where the last call ended. 
 * 
 * @return The first (or next in subsequent calls) token in the string.
 */
//------------------------------------------------------------------------------
std::string Interpreter::GetToken(std::string tokstr)
{
    static std::string str, token = "";
    static Integer start;
    if (tokstr != "") {
        str = tokstr;
        start = 0;
    }

    if (start >= (Integer)strlen(str.c_str()))
        token = "";
    else {
        Integer loc = str.find(".", start);
        if (loc == (Integer)std::string::npos)
            loc = strlen(str.c_str());
        token.assign(str, start, loc-start);
        start = loc+1;
    }
    
    return token;
}


//------------------------------------------------------------------------------
// GmatBase* FindObject(std::string objName)
//------------------------------------------------------------------------------
/**
 * Finds a previously created named object.
 * 
 * @param objName The name of the object of interest.
 * 
 * @return Pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::FindObject(std::string objName)
{
    GmatBase *obj = moderator->GetConfiguredItem(objName);
    return obj;
}


//------------------------------------------------------------------------------
// bool SetParameter(GmatBase *obj, Integer id, std::string value)
//------------------------------------------------------------------------------
/**
 * Sets parameters on GMAT objects.
 * 
 * @param obj   Pointer to the object that owns the parameter.
 * @param id    ID for the parameter.
 * @param value Value of the parameter.
 * 
 * @return true if the parameter is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Interpreter::SetParameter(GmatBase *obj, Integer id, std::string value)
{
    bool retval = false;
    
    Gmat::ParameterType type = obj->GetParameterType(id);
    if (type == Gmat::INTEGER_TYPE){
        obj->SetIntegerParameter(id, atoi(value.c_str()));
        retval = true;
    }
    if (type == Gmat::REAL_TYPE) {
        obj->SetRealParameter(id, atof(value.c_str()));
        retval = true;
    }
    if (type == Gmat::STRING_TYPE) {
        obj->SetStringParameter(id, value);
        retval = true;
    }

    return retval;
}


//bool Interpreter::CheckMemberObject(GmatBase *obj, std::string memberID)
//{
//    return false;
//}

