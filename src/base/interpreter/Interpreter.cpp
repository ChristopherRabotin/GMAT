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

#include <ctype.h>         // for isalpha


//#define DEBUG_TOKEN_PARSING 1
//#define DEBUG_RHS_PARSING 1


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
    RegisterAliases();
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
    
    if (objecttype == "Formation") {
        CreateFormation(objectname);
        return true;
    }
    
    if (objecttype == "Propagator") {
        CreatePropSetup(objectname);
        return true;
    }

    if (objecttype == "ForceModel") {
        CreateForceModel(objectname);
        return true;
    }
    
    if (objecttype == "ImpulsiveBurn") {
        CreateBurn(objectname, true);
        return true;
    }

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
               
    std::string prefix = "GMAT ";
    prefix += objectname;
    prefix += ".";

    WriteParameters(prefix, obj);
    
    *outstream << "\n";
    return true;
}


//------------------------------------------------------------------------------
// void WriteParameters(std::string &prefix, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Code that writes the parameter details for an object.
 * 
 * @param prefix Starting portion of the script string used for the parameter.
 * @param obj The object that is written.
 */
//------------------------------------------------------------------------------
void Interpreter::WriteParameters(std::string &prefix, GmatBase *obj)
{
    if (obj == NULL)
       return;

    Integer i;
    for (i = 0; i < obj->GetParameterCount(); ++i) 
    {
        // Handle StringArray parameters separately
        if (obj->GetParameterType(i) != Gmat::STRINGARRAY_TYPE) {
           // Fill in the l.h.s.
           *outstream << prefix << obj->GetParameterText(i) << " = ";
           WriteParameterValue(obj, i);
           *outstream << ";\n";
        }
        else {
           *outstream << prefix << obj->GetParameterText(i) << " = {";
           StringArray sar = obj->GetStringArrayParameter(i);
           for (StringArray::iterator n = sar.begin(); n != sar.end(); ++n) {
              if (n != sar.begin())
                 *outstream << ", ";
              *outstream << (*n);
           }
           *outstream << "};\n";
        }
    }

    GmatBase *ownedObject;
    std::string nomme, newprefix;
    for (i = 0; i < obj->GetOwnedObjectCount(); ++i) {
       newprefix = prefix;
       ownedObject = obj->GetOwnedObject(i);
       nomme = ownedObject->GetName();
       if (nomme != "") {
          newprefix += ".";
          newprefix += nomme;
       }
       WriteParameters(newprefix, ownedObject);
    }
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
            
        case Gmat::UNSIGNED_INT_TYPE:
            *outstream << obj->GetUnsignedIntParameter(id);
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
    return (Spacecraft *)(moderator->CreateSpacecraft("Spacecraft", satname));
}


//------------------------------------------------------------------------------
// Formation* CreateFormation(std::string formname)
//------------------------------------------------------------------------------
/**
 * Calls the Moderator to create a new Formation object.
 * 
 * @param satname Name of the formation.
 * 
 * @return Pointer to the constructed Formation.
 */
//------------------------------------------------------------------------------
Formation* Interpreter::CreateFormation(std::string formname)
{
    return (Formation *)(moderator->CreateSpacecraft("Formation", formname));
}


//------------------------------------------------------------------------------
// bool AssembleCommand(const std::string& scriptline)
//------------------------------------------------------------------------------
/**
 * Builds a command and sets the internal data based on the script test.
 * 
 * @param scriptline The line of script that generated the command.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Interpreter::AssembleCommand(const std::string& scriptline, GmatCommand *cmd)
{
   StringArray topLevel = Decompose(scriptline);
   StringArray sublevel[10];  // Allow up to 10 deep for now
   Integer     cl = 0;        // Current level of decomposition
   GmatBase    *object[10];   // Up to 10 reference objects
   GmatBase    *temp;
   Integer     ol = 0;        // Current object level (depth when drilling into 
                              // owned objects)
   Gmat::ObjectType type;
   
   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "Top level command decomposition:\n   size = " 
                << topLevel.size() << "\n   ";
      for (StringArray::iterator i = topLevel.begin(); i != topLevel.end(); ++i)
         std::cout << *i << "\n   ";
      std::cout << std::endl;
   #endif
   
   // First construct the Command if the input is NULL
   if (cmd == NULL) {
      if (find(cmdmap.begin(), cmdmap.end(), topLevel[0]) != cmdmap.end()) {
         //cmd = CreateCommand(topLevel[0]);
         GmatCommand *cmd = moderator->AppendCommand(topLevel[0], "");
         if (cmd == NULL)
            throw InterpreterException("Cannot create \"" + topLevel[0] + 
                                       "\" command.");
      }
      else 
         throw InterpreterException("\"" + topLevel[0] + 
                                    "\" command not recognized.");
   }
   if (!cmd)  
      return false;

   cmd->SetGeneratingString(scriptline);
      
   // Perform Command specific tasks: Setting string data, and so forth
   std::string cmdCase = cmd->GetTypeName();
   Integer condNumber = 1, index = 0;
   if ((cmdCase == "If") || (cmdCase == "While")) {
      // chain through the conditions
      while (condNumber < (Integer)topLevel.size()) {
         #ifdef DEBUG_TOKEN_PARSING
            std::cout << "Setting If condition:\n   lhs = " 
                      << topLevel[condNumber]
                      << "\n   op  = " << topLevel[condNumber+1]
                      << "\n   rhs = " << topLevel[condNumber+2] 
                      << std::endl;
         #endif
         if (!cmd->SetCondition(topLevel[condNumber], topLevel[condNumber+1], 
                                topLevel[condNumber+2]))
               throw CommandException("Cannot set conditions on command " + 
                                      scriptline);
         if (condNumber+3 < (Integer)topLevel.size()) {
            #ifdef DEBUG_TOKEN_PARSING
               std::cout << "Setting If condition operator: "
                         << topLevel[condNumber+3] << std::endl;
            #endif
            if (!cmd->SetConditionOperator(topLevel[condNumber+3]))
               throw CommandException("Cannot set condition operator on command " + 
                                      scriptline);
         }
      
         sublevel[cl] = Decompose(topLevel[condNumber]);
         #ifdef DEBUG_TOKEN_PARSING
            std::cout << "Decomposing \"" << topLevel[condNumber] << "\"\n";
         #endif
         if (sublevel[cl].size() == 1) {
            // Size 1 implies an object reference or parm string
            object[ol] = moderator->GetConfiguredItem(sublevel[cl][0]);
            if (object[ol]) {
               type = object[ol]->GetType();
               try {
                  if (!cmd->SetRefObjectName(type, object[ol]->GetName()))
                     throw InterpreterException("Cannot set object " + object[ol]->GetName() + 
                                                " for command " + (cmd->GetTypeName()));
               }
               catch (BaseException &ex) {
                  if (!cmd->SetRefObject(object[ol], type, object[ol]->GetName(), 0))
                     throw CommandException("Cannot set reference object " + 
                                            object[ol]->GetName());
               }
            }
            else {
               
            }
         }
         else {
            temp = AssemblePhrase(sublevel[cl], cmd);
            if (!temp)
               throw InterpreterException("Cannot Assemble construct " + 
                                          sublevel[cl][0] + " for command " + 
                                          (cmd->GetTypeName()));
            temp->SetName(topLevel[condNumber]);
            cmd->SetRefObject(temp, temp->GetType(), temp->GetName(), index);
         }
         condNumber += 4;
         ++index;
      }

      return true;
   }
   
   if (cmdCase == "For") {
      Real start, step = 1.0, stop;
      
      // Find the start and end values, and the step if one is specified
      StringArray::iterator w;
      
      #ifdef DEBUG_TOKEN_PARSING
         std::cout << "Parsing " << scriptline << "\n   ";
         for (w = topLevel.begin()+1; w != topLevel.end(); ++w)
             std::cout << *w << "\n   ";
         std::cout << std::endl;
      #endif
      
      // First token is the loop index
      w = topLevel.begin()+1;
      // Parameter *parm = moderator->CreateParameter(*w);
      // cmd->SetRefObject(Gmat::PARAMETER, parm);
      ++w;
      if (*w != "=")
         throw InterpreterException("For loop missing \"=\" character");
      ++w;
      start = atof(w->c_str());
      ++w;
      if (*w != ":")
         throw InterpreterException("For loop missing first \":\" character");
      ++w;
      stop = atof(w->c_str());
      ++w;
      
      if (w != topLevel.end()) {
         if (*w == ":") {
            step = stop;   
            ++w;
            stop = atof(w->c_str());
         }
         // Commented out because of the possibility of trailing white space
         // else
         //    throw InterpreterException("For loop missing second \":\" character");
      }
      
      #ifdef DEBUG_TOKEN_PARSING
         std::cout << "Setting values:\n   start = " << start 
                   << "\n   step  = " << step 
                   << "\n   stop  = " << stop 
                   << std::endl;
      #endif
      
      cmd->SetRealParameter("StartValue", start);
      cmd->SetRealParameter("Step", step);
      cmd->SetRealParameter("EndValue", stop);
      return true;
   }
      
   for (StringArray::iterator i = topLevel.begin()+1; i != topLevel.end(); ++i) {
      // If we see a comment character, we're done
      if ((*i)[0] == '%')
         break;
         
      // Walk through the rest of the command, setting it up
      sublevel[cl] = Decompose(*i);
      #ifdef DEBUG_TOKEN_PARSING
         std::cout << "Decomposing \"" << (*i) << "\"\n";
      #endif
      if (sublevel[cl].size() == 1) {
         // Size 1 implies an object reference or parm string
         object[ol] = moderator->GetConfiguredItem(*i);
         if (object[ol]) {
            type = object[ol]->GetType();
            try {
               if (!cmd->SetRefObjectName(type, *i))
                  throw InterpreterException("Cannot set object " + (*i) + 
                                             " for command " + (cmd->GetTypeName()));
            }
            catch (BaseException &ex) {
               if (!cmd->SetRefObject(object[ol], type, object[ol]->GetName(), 0))
                  throw;
            }
         }
         else {
            throw InterpreterException("Cannot find object named " + *i);
         }
      }
      else {
         if (!AssemblePhrase(sublevel[cl], cmd))
            return false;
      }
   }
   return true;
}


GmatBase* Interpreter::AssemblePhrase(StringArray& phrase, GmatCommand *cmd)
{
   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "Entered Interpreter::AssemblePhrase\n";
   #endif
   StringArray breakdown;
   GmatBase *ref = NULL;
   ObjectArray objectlist;
   if (phrase.size() == 1) {

      breakdown = Decompose(phrase[0]);
      if (breakdown.size() == 1) {
         ref = moderator->GetConfiguredItem(phrase[0]);
      }
      else {
         for (StringArray::iterator i = phrase.begin(); i != phrase.end(); ++i) {
            #ifdef DEBUG_TOKEN_PARSING
               std::cout << "   Breaking down subphrase " << *i << "\n";
            #endif
            breakdown = Decompose(*i);
            if (breakdown.size() == 1) {
               ref = moderator->GetConfiguredItem(*i);
            }
            else {
               #ifdef DEBUG_TOKEN_PARSING
                  std::cout << "   Size != 1\n";
               #endif
            }
         }
      }
   }
   // Not an object reference -- check the other options
   else {
      // Is it a Parameter?
      if (phrase.size() == 2) {
         ref = moderator->GetParameter(phrase[1]);
         if (ref == NULL) {
            std::string name = phrase[0];
            name += ".";
            name += phrase[1];
            ref = moderator->CreateParameter(phrase[1], name);
            if (ref) {
               #ifdef DEBUG_TOKEN_PARSING
                  std::cout << "Parameter configuration:\n   Parm = " << phrase[1]
                            << "\n   obj  = " << phrase[0] << std::endl; 
               #endif
               GmatBase *refobj = moderator->GetConfiguredItem(phrase[0]);
               Gmat::ObjectType ot = refobj->GetType();
               ref->SetRefObjectName(ot, phrase[0]);
            }
         }
         else // NOT a Parameter
            throw InterpreterException("Could not parse " + phrase[1]);
      }
   }
   return ref;
}


//GmatBase* Interpreter::AssemblePhrase(StringArray& phrase, GmatCommand *cmd)
//{
//std::cout << "Entered Interpreter::AssemblePhrase\n";
//   StringArray breakdown;
//   GmatBase *ref = NULL;
//   for (StringArray::iterator i = phrase.begin(); i != phrase.end(); ++i) {
//std::cout << "   Breaking down subphrase " << *i << "\n";
//      breakdown = Decompose(*i);
//      if (breakdown.size() == 1) {
//         ref = moderator->GetConfiguredItem(*i);
//      }
//      else {
//std::cout << "   Size != 1\n";
////         ref = BuildMultipartItem();
//      }
//   }
//   return ref;
//}
//
//
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
//                                 StringArray::iterator& phrase, Integer index)
//------------------------------------------------------------------------------
/**
 * Sets PropSetup parameters.
 * 
 * @param obj Object that is being configured.
 * @param phrase Phrase containing the configuration information.
 * 
 * @return true if the parameter is set, false otherwise.
 * 
 * @note This method is deprecated and remains in the code until it can be 
 *       safely removed.
 */
//------------------------------------------------------------------------------
bool Interpreter::InterpretPropSetupParameter(GmatBase *obj, 
                                              StringArray& items,
                                              std::vector<std::string*>::iterator& phrase,
                                              Integer index)
{
   bool retval = true;
   // Set object associations
   std::string objParm = items[index];

   try {
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
   }
   catch (BaseException &ex) {
      /// Check to see if it is a propagator parameter; otherwise throw
      Propagator *prop = ((PropSetup*)obj)->GetPropagator();
      if (prop == NULL)
         throw;
      ++phrase;
       
      if (**phrase == "=")
         ++phrase;
      else
         throw InterpreterException("Syntax error creating Propagator");
         
      Integer id = prop->GetParameterID(objParm);
#ifdef DEBUG_INTERPRETER
      std::cout << "Setting " << objParm << " on " << prop->GetTypeName() << " to " << **phrase << "\n";
#endif
      if (!SetParameter(prop, id, **phrase))
         throw;
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
        if (IsGroup(&(str[start]))) {
           char endchar = ';';
           if (str[start] == '[')
              endchar = ']';
           if (str[start] == '(')
              endchar = ')';
           if (str[start] == '{')
              endchar = '}';
           while (str[end] != endchar) {
              ++end;  
              if (str[end] == '\0')
                 throw InterpreterException("Missing closing character");
           }
           ++end;
        }
        else {
           while ((str[end] != ' ') && (str[end] != '\t') && (str[end] != '\r') &&
                  (str[end] != '\n') && (str[end] != '%') && (str[end] != '\0')) {
               ++end;
               if ((str[end] == ';') && (semicolonLocation == 0))
                   semicolonLocation = end;
           }
        }
        if (semicolonLocation == 0)
            semicolonLocation = end;
        
        phrase.assign(line, start, (semicolonLocation-start));
        
        chunks.push_back(new std::string(phrase));
        start = SkipWhiteSpace(end);
        semicolonLocation = 0;
    }
}


bool Interpreter::IsGroup(const char *text)
{
   if ((text[0] == '[') || (text[0] == '(') || (text[0] == '{'))
      return true;
   return false;
}


//------------------------------------------------------------------------------
// Integer SkipWhiteSpace(Integer start, const std::string &text)
//------------------------------------------------------------------------------
/**
 * Skips blank spaces and tabs.
 * 
 * @param start Starting point in the script line.
 * @param text The string under analysis.
 * 
 * @return Location of the first useful character, or -1 for comments or line
 *         endings.
 */
//------------------------------------------------------------------------------
Integer Interpreter::SkipWhiteSpace(Integer start, const std::string &text)
{
    Integer finish = start;
    const char *str;
    
    if (text == "")
       str = line.c_str();
    else
       str = text.c_str();
       
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
// Integer FindDelimiter(std::string &str, std::string &specChar)
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
Integer Interpreter::FindDelimiter(const std::string &str, const std::string &specChar)
{
    return -1;
}


//------------------------------------------------------------------------------
// std::string GetToken(std::string &tokstr)
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
std::string Interpreter::GetToken(const std::string &tokstr)
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
// StringArray& Decompose(const std::string &chunk)
//------------------------------------------------------------------------------
/**
 * Breaks a grouped string into component pieces.
 * 
 * @param chunk The string.
 * 
 * @return An array containing the pieces.
 */
//------------------------------------------------------------------------------
StringArray& Interpreter::Decompose(const std::string &chunk)
{
   static StringArray elements;
   elements.clear();
   
   // Branch based on the first character in the string
   char branchtype = (chunk.c_str())[0];
   
   if (branchtype == '(') {
      // todo: Check to be sure we have closing paren
      elements = SeparateParens(chunk);
   }
   else if (branchtype == '[') {
      // todo: Check to be sure we have closing bracket
      elements = SeparateBrackets(chunk);
   }
   else if (branchtype == '{') {
      // todo: Check to be sure we have closing brace
      elements = SeparateBraces(chunk);
   }
   else if ((chunk.find(" ", 0) == std::string::npos) && 
            (chunk.find(",", 0) == std::string::npos)) {
      elements = SeparateDots(chunk, ".");
   }
   else {
      elements = SeparateSpaces(chunk);
   }
   
   return elements;
}


//------------------------------------------------------------------------------
// StringArray& SeparateSpaces(const std::string &chunk)
//------------------------------------------------------------------------------
/**
 * Breaks a string containing white space into component pieces.
 * 
 * In this context, "white space" is the space character or the tab character.  
 * 
 * Note that this is not the most efficient code for this function -- other 
 * techniques are likely to have better performance.  Instead, this method is 
 * written to allow for other types of white space by changing the member 
 * variables whitespace[] and wSpaceTypes.
 * 
 * @param chunk The string.
 * 
 * @return An array containing the pieces.
 * 
 * @note This method is not yet implemented.  It will be completed during 
 *       build 3.
 */
//------------------------------------------------------------------------------
StringArray& Interpreter::SeparateSpaces(const std::string &chunk)
{
   static StringArray chunkArray;
   chunkArray.clear();
   char closebrace = '\0';
   std::string token;
   Integer pos = 0, end;

   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "In SeparateSpaces: string = \"" << chunk << "\"\n   ";
   #endif

   // Examine the string character by character, skipping grouping elements
   char str[4096];
   strcpy(str, chunk.c_str());
   for (unsigned i = 0; i < chunk.length(); ++i) {
      if ((str[i] == '{') || (str[i] == '(') || (str[i] == '[')) {
         #ifdef DEBUG_TOKEN_PARSING
            std::cout << " found \"" << str[i] << "\" ";
         #endif
         token.assign(str, pos, i-pos);
         chunkArray.push_back(token);
         pos = i;
         // Now find and push the bracketed chunk
         if (str[i] == '{')
            closebrace = '}';
         if (str[i] == '(')
            closebrace = ')';
         if (str[i] == '[')
            closebrace = ']';
         while (str[i] != closebrace) {
            ++i;
            if (i > chunk.length())
               throw InterpreterException("Missing closing brace in line " + chunk);
         }
         token.assign(str, pos, i-pos+1);
         chunkArray.push_back(token);
//         ++i;
         pos = i+1;
      }
      else if ((str[i] == '<') || (str[i] == '>') || (str[i] == '~')) {
         #ifdef DEBUG_TOKEN_PARSING
            std::cout << " found \"" << str[i] << "\" ";
         #endif
         if (str[i+1] == '=') {
            ++i;
         }
         token.assign(str, pos, i-pos+1);
         chunkArray.push_back(token);
//         ++i;
         pos = i+1;
      }
      else if ((str[i] == ' ') || (str[i] == ',') ||
               (str[i] == '=') || (str[i] == ':') ) {
         #ifdef DEBUG_TOKEN_PARSING
            std::cout << " found \"" << str[i] << "\" ";
         #endif
         token.assign(str, pos, i-pos);
         if ((token != " ") && (token.length() > 0))
            chunkArray.push_back(token);
         if ((str[i] == '=') || (str[i] == ':')) {
            token.assign(str, i, 1);
            chunkArray.push_back(token);
         }
//         ++i;
         pos = i+1;
      }
      #ifdef DEBUG_TOKEN_PARSING
         std::cout << "str[i] = \"" << str[i] << "\"; \"" << token 
                   << "\" found\n   ";
      #endif
   }
   // Push the last piece onto the array, unless it is a semicolon
   token.assign(str, pos, chunk.length());
   if (token.find(";", 0) != std::string::npos) {
      end = token.find(";", 0);
      token.assign(token, 0, end);
   }
   if ((token != ";") && (token != ""))
         chunkArray.push_back(token);   
         
   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "Strings parsed:\n   ";
      for (StringArray::iterator i = chunkArray.begin(); i != chunkArray.end(); 
           ++i)
         std::cout << *i << "\n   ";
      std::cout << std::endl;
   #endif

   return chunkArray;
}


//------------------------------------------------------------------------------
// StringArray& SeparateDots(const std::string &chunk, 
//                           const std::string &delimiter)
//------------------------------------------------------------------------------
/**
 * Breaks a period separated string into component pieces.
 * 
 * This method takes an input string containing heirarchical data separated by a
 * specified delimiter (a period by default) and fills a StringArray with the
 * data in the string.  For example, the input string "fm.GravityField.Earth"
 * places three strings in the array: "fm", "GravityField", and "Earth".
 * 
 * @param chunk The string.
 * @param delimiter The character that separates the pieces -- a "." by default.
 * 
 * @return An array containing the pieces.
 */
//------------------------------------------------------------------------------
StringArray& Interpreter::SeparateDots(const std::string &chunk, 
                                       const std::string &delimiter)
{
   static StringArray chunkArray;
   chunkArray.clear();
   Integer loc, start = 0;
   std::string token = "Starting", str = chunk;
   bool parseComplete = false;
   
   while (!parseComplete) {
      loc = chunk.find(delimiter, start);
      if (loc == (Integer)std::string::npos) {
         loc = strlen(str.c_str());
         parseComplete = true;
      }
      token.assign(str, start, loc-start);
      start = loc+strlen(delimiter.c_str());
      if (token != "")
         chunkArray.push_back(token);
   }
   
   return chunkArray;
}


//------------------------------------------------------------------------------
// StringArray& SeparateParens(const std::string &chunk)
//------------------------------------------------------------------------------
/**
 * Breaks a string contained in parentheses into component pieces.
 * 
 * @param chunk The string.
 * 
 * @return An array containing the pieces.
 */
//------------------------------------------------------------------------------
StringArray& Interpreter::SeparateParens(const std::string &chunk)
{
   static StringArray chunkArray;
   chunkArray.clear();
   return chunkArray;
}


//------------------------------------------------------------------------------
// StringArray& SeparateBraces(const std::string &chunk)
//------------------------------------------------------------------------------
/**
 * Breaks a string contained in curly braces ("{...}") into component pieces.
 * 
 * @param chunk The string.
 * 
 * @return An array containing the pieces.
 */
//------------------------------------------------------------------------------
StringArray& Interpreter::SeparateBraces(const std::string &chunk)
{
   static StringArray chunkArray;
   chunkArray.clear();

   Integer loc, start = chunk.find("{", 0), stop = chunk.find("}", 0);
   bool parseComplete = false;
   
   if (start == (Integer)std::string::npos) 
         parseComplete = true;
   else if (stop == (Integer)std::string::npos) 
      throw InterpreterException("Missing closing brace \"}\"");

   if (start > stop)
      throw InterpreterException("Closing brace found before opening brace");
      
   loc = start;
      
   std::string token = "Starting", str = chunk;
   
   while (!parseComplete && (start < stop)) {
      // skip over white spaces
      start = loc + 1;
      start = SkipWhiteSpace(start, str);
      loc = chunk.find(",", start);
      if (loc == (Integer)std::string::npos) {
         loc = stop;
         parseComplete = true;
      }
      token.assign(str, start, loc-start);
      
      // Prep for the next token
      if (token != "")
         chunkArray.push_back(token);
   }
      
   return chunkArray;
}


//------------------------------------------------------------------------------
// StringArray& SeparateBrackets(const std::string &chunk)
//------------------------------------------------------------------------------
/**
 * Breaks a string contained in square brackets ("[...]") into component pieces.
 * 
 * @param chunk The string.
 * 
 * @return An array containing the pieces.
 */
//------------------------------------------------------------------------------
StringArray& Interpreter::SeparateBrackets(const std::string &chunk)
{
   static StringArray chunkArray;
   chunkArray.clear();
   return chunkArray;
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
// GmatBase* FindOwnedObject(StringArray TokenList, Integer index)
//------------------------------------------------------------------------------
/**
 * Finds an object wrapped in another object.
 * 
 * @param objName The name of the object of interest.
 * 
 * @return Pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* Interpreter::FindOwnedObject(StringArray tokenList, GmatBase *owner,
                                       Integer index)
{
   GmatBase* obj = NULL;
   Integer count = tokenList.size(), i = 0;
   
   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "Current owner is \"" << owner->GetName() 
                << "\" of type \"" << owner->GetTypeName()  
                << "\"\n";
      std::cout << "Token list:\n   ";
      for (StringArray::iterator ix = tokenList.begin(); ix != tokenList.end(); ++ix)
         std::cout << *ix << "\n   ";
      std::cout << "\n";
   #endif
   
   GmatBase* topOwner = FindObject(tokenList[0]);
   if (topOwner->GetType() != Gmat::FORCE_MODEL)
      throw InterpreterException("The ForceModel is the only allowed owner\n" + 
                                 line);
   
   ObjectArray objs = owner->GetRefObjectArray(tokenList[index]);
   if (index == count - 2) {

      #ifdef DEBUG_TOKEN_PARSING
            std::cout << "Looking for " << tokenList[index] << "\n";
      #endif

      if (tokenList[index] == owner->GetName())
         obj = owner;
      else {
         if (objs.size() == 0) {
            std::string errstr = "Object list is empty; ";
            errstr += "cannot parse the line\n  \"";
            errstr += line;
            errstr += "\"";
            throw InterpreterException(errstr);
         }
         if (objs.size() > 1) {
            for (ObjectArray::iterator j = objs.begin(); j != objs.end(); ++j) {
               Integer id = (*j)->GetParameterID("BodyName");
               if (id != -1) {
                  std::string bodyName = (*j)->GetStringParameter(id);
                  if (bodyName == tokenList[index+1])
                     obj = *j;
               }
            }
         }
         else
            obj = objs[0];
      }
   }
   else {
      if (objs.size() == 0)
         throw InterpreterException("Could not find objects of type \"" + 
                                    tokenList[index] + "\"");
      obj = FindOwnedObject(tokenList, objs[i], index+1); 
   }
   
   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "Returning a \"" << obj->GetTypeName() << "\" object"
                << "named \"" << obj->GetName() << "\"\n";   
   #endif

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
    
    std::string valueToUse = value;
    CheckForSpecialCase(obj, id, valueToUse);
    
    Gmat::ParameterType type = obj->GetParameterType(id);
    if (type == Gmat::INTEGER_TYPE){
        obj->SetIntegerParameter(id, atoi(valueToUse.c_str()));
        retval = true;
    }
    if (type == Gmat::REAL_TYPE) {
        obj->SetRealParameter(id, atof(valueToUse.c_str()));
        retval = true;
    }
    if ((type == Gmat::STRING_TYPE) || (type == Gmat::STRINGARRAY_TYPE)) {
        retval = obj->SetStringParameter(id, valueToUse);
    }

    if (type == Gmat::BOOLEAN_TYPE) {
       bool tf;
       if (value == "true")
          tf = true;
       else
          tf = false;
       obj->SetBooleanParameter(id, tf);
       retval = true;
    }
    
    return retval;
}


//------------------------------------------------------------------------------
// void CheckForSpecialCase(GmatBase *obj, Integer id, std::string& value)
//------------------------------------------------------------------------------
void Interpreter::CheckForSpecialCase(GmatBase *obj, Integer id, 
                                      std::string& value)
{
   std::string val = value;

   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "Entered CheckForSpecialCase with \"" << value
                << "\" being set on parameter \"" << obj->GetParameterText(id)
                << "\" for a \"" << obj->GetTypeName() << "\" object\n";
   #endif
   
   // JGM2 and JGM3 are special strings  in GMAT; handle them here
   if ((obj->GetTypeName() == "GravityField") &&
       (obj->GetParameterText(id) == "Filename")) {
      val = moderator->GetPotentialFileName(value);
      if (val.find("Unknown Potential File Type") == std::string::npos)
         value = val;
   }

   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "Leaving CheckForSpecialCase with \"" << value << "\"\n";
   #endif
}


//bool Interpreter::CheckMemberObject(GmatBase *obj, std::string memberID)
//{
//    return false;
//}

void Interpreter::RegisterAliases(void)
{
   ForceModel::SetScriptAlias("PrimaryBodies", "GravityField");
   ForceModel::SetScriptAlias("Gravity", "GravityField");
   ForceModel::SetScriptAlias("PointMasses", "PointMassForce");
   ForceModel::SetScriptAlias("Drag", "DragForce");
   ForceModel::SetScriptAlias("SRP", "SolarRadiationPressure");
}


bool Interpreter::ConfigureForce(ForceModel *obj, std::string& objParm, 
                                 std::string& parm)
{
   bool retval = false;
   
   #ifdef DEBUG_TOKEN_PARSING
      std::cout << "Interpreter::ConfigureForce entered for line\n  \""
                << line << "\"\n  with parm = \"" << objParm <<"\" = \""
                << parm << "\"\n";
   #endif
   
   std::string forcetype = ForceModel::GetScriptAlias(objParm);
   if (forcetype == "SolarRadiationPressure")
      if (parm != "On")
         return true;
   if (forcetype == "DragForce")
      if (parm == "None")
         return true;
   PhysicalModel *pm = moderator->CreatePhysicalModel(forcetype, "");
   
   if (pm) {
      pm->SetName(parm);
      if (!pm->SetStringParameter("BodyName", parm))
         if ((forcetype == "GravityField") || (forcetype == "PointMassForce"))
            throw InterpreterException("Unable to set body for force " + objParm);
      if (forcetype == "DragForce") {
         if (!pm->SetStringParameter("AtmosphereModel", parm))
            throw InterpreterException("Unable to set AtmosphereModel for drag force.");
         /// @todo Add the body name for drag at other bodies
         if (parm != "BodyDefault") {
            //---------------------------------------------------------------
            //loj: 10/25/04 Set "Earth" as AtmosphereBody until it can handle
            // Drag.Earth. Above pm->SetStringParameter("BodyName", parm) sets
            // AtmosphereModel as Bodyname.
            //---------------------------------------------------------------
            pm->SetStringParameter("BodyName", "Earth");
            AtmosphereModel *m = moderator->CreateAtmosphereModel(parm, "");
            if (m)
               pm->SetRefObject(m, Gmat::ATMOSPHERE);
         }
      }
      if (forcetype == "GravityField") {
         std::string potFilename = moderator->GetPotentialFileName("JGM2");
         if (potFilename == "") {
            // No file name set, so set to a default value
            potFilename = "./files/gravity/earth/JGM2.grv";
            MessageInterface::ShowMessage("No potential file set, so using \"%s\"",
                                          potFilename.c_str());
         }
         if (!pm->SetStringParameter("Filename", potFilename))
            throw InterpreterException("Unable to set full field model file.");
      }
   
      obj->AddForce(pm);
      retval = true;
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool ConstructRHS(GmatBase *lhsObject, const std::string& rhs, 
//                   const std::string& label)
//------------------------------------------------------------------------------
/**
 * Builds the right side of an assignment line (e.g. "GMAT plot.Add = sat.X;")
 * 
 * @param lhsObject The object on the left that receives the parameters.
 * @param rhs       The string to the right of the equals sign.
 * @param label     The string used to register the rhs (e.g. "Add").
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Interpreter::ConstructRHS(GmatBase *lhsObject, const std::string& rhs, 
                               const std::string& label)
{
   #ifdef DEBUG_RHS_PARSING
      MessageInterface::ShowMessage("%s%s%s%s%s%s\"\n",
                                    "Interpreter::ConstructRHS called with string \"",
                                    rhs.c_str(), 
                                    "\" registered using \"",
                                    label.c_str(),
                                    "\" for object \"", 
                                    lhsObject->GetName().c_str());
   #endif
   
   StringArray sar = Decompose(rhs);
   if ((sar.size() > 1) && isalpha(sar[1][0])) {
      if (!IsGroup(rhs.c_str())) {
         try {
            std::string name = rhs;
            #ifdef DEBUG_RHS_PARSING
               MessageInterface::ShowMessage("%s%s\"\n",
                                             "Attempting to build parameter \"",
                                             sar[1].c_str());
            #endif
            Parameter *parm = CreateParameter(name, sar[1]);
            if (parm != NULL) {
               GmatBase *parmObj = FindObject(sar[0]);
               std::string parmtype = "Object";
               if (parmObj->GetType() == Gmat::SPACECRAFT)
                  parmtype = "Spacecraft";
               #ifdef DEBUG_RHS_PARSING
                  MessageInterface::ShowMessage("%s%s%s%s%s%s\"\n",
                                                "Assigning \"",
                                                sar[0].c_str(), 
                                                "\" to parameter \"", 
                                                parm->GetName().c_str(),
                                                "\" with descriptor \"",
                                                parmtype.c_str());
               #endif
               parm->SetStringParameter(parmtype, sar[0]);
               lhsObject->SetStringParameter(label, name);
               return true;
            }
         }
         catch (BaseException &ex)
         {
            #ifdef DEBUG_RHS_PARSING
               MessageInterface::ShowMessage("Parameter was not constructed.\n");
            #endif
         }
      }
   }
   
   return false;
}
