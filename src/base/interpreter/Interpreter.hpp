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
 * Class definition for the Interpreter base class
 */
//------------------------------------------------------------------------------


#ifndef Interpreter_hpp
#define Interpreter_hpp

#include <iostream>
#include <string>
#include <map>

#include "gmatdefs.hpp"
#include "InterpreterException.hpp"
#include "GmatBase.hpp"

// Forward references for GMAT core objects
class Spacecraft;
class Formation;
class Propagator;
class ForceModel;
class PropSetup;
class PhysicalModel;
class SolarSystem;
class CelestialBody;
class Parameter;
class GmatCommand;
class Publisher;
class Subscriber;
class Burn;


//#include "Moderator.hpp"
class Moderator;


/**
 * Interpreter is the base class for the GMAT Interpreter subsystem.  
 * 
 * Interpreter defines the interfaces used to parse the text files that control 
 * execution in GMAT.  It also provides the interfaces to write text files out, 
 * either to the file system, the screen, or the GUI.
 */
class Interpreter
{
public:
   Interpreter();
   virtual ~Interpreter();
    
   bool                            SetInStream(std::istream *str);
   bool                            SetOutStream(std::ostream *str);
    
   // The "Interpret" methods translate text streams (e.g. scripts and 
   // subscripts) into GMAT objects 
   virtual bool                    Interpret(void) = 0;
   bool                            InterpretObject(std::string objecttype,
                                                    std::string objectname);
    
   // The "Build" methods take GMAT objects and write out scripts or subscripts
   virtual bool                    Build(void) = 0;
   bool                            BuildObject(std::string &objectname);

protected:
   /// The input stream processed by the interpreter
   std::istream                    *instream;
   /// The iostream processed by the interpreter
   std::ostream                    *outstream;
   /// Pointer to the Moderator for access to the factories and other services
   Moderator                       *moderator;
   /// Flag to tell is the mappings are defined yet
   bool                            initialized;
    
   /// Mapping between the object base class strings and the objecttypes enum
   std::map<std::string, Integer>  typemap;
    
   /// Available commands, obtained from the FactoryManager via the Moderator
   StringArray                     cmdmap;
   /// Available propagators
   StringArray                     propmap;
   /// Available forces
   StringArray                     forcemap;
   /// Available subscribers
   StringArray                     subscribermap;
   /// Available parameters
   StringArray                     parametermap;
   /// Available stopping conditions
   StringArray                     stopcondmap;
   /// Available solvers
   StringArray                     solvermap;

   /// Current line from the script
   std::string                     line;
   std::vector<std::string *>      chunks;
    
   /// Identifies the different types of script lines possible
   enum linetype               { CREATEOBJECT = 7001,
                                 OBJECTPARM,
                                 CONFIGURATION,
                                 TIMELINE,
                                 BLANKLINE,
                                 UNKNOWN };


   void                          Initialize(void);

   // The "Create" methods make calls, through the Moderator, to the Factories
   // to get new instances of the requested objects
   Spacecraft*                   CreateSpacecraft(std::string satname);
   Formation*                    CreateFormation(std::string formname);
   GmatCommand*                  CreateCommand(std::string commandtype);
   Propagator*                   CreatePropagator(std::string proptype);
   ForceModel*                   CreateForceModel(std::string modelname);
   PhysicalModel*                CreatePhysicalModel(std::string forcetype);
   SolarSystem*                  CreateSolarSystem(std::string ssname);
   CelestialBody*                CreateCelestialBody(std::string cbname, 
                                                     std::string type);
   Parameter*                    CreateParameter(std::string name, 
                                                    std::string type);
//   StoppingCondition*            CreateStopCond(std::string conditiontype);
   Subscriber*                   CreateSubscriber(std::string name, 
                                                  std::string type);
   Burn*                         CreateBurn(std::string satname, bool isImpulsive = false);
                                                    
   // The following method signature depends on an open scripting issue: if
   // props and force models are named, the following Create method should use
   // the names rather than the object pointers
   PropSetup*                    CreatePropSetup(std::string name);
    
   bool                          AssembleCommand(const std::string& scriptline,
                                                 GmatCommand *cmd = NULL);
   GmatBase*                     AssemblePhrase(StringArray& phrase,
                                                GmatCommand *cmd);
   
   bool                          InterpretPropSetupParameter(GmatBase *obj, 
                                                StringArray& items,
                                                std::vector<std::string*>::iterator& phrase, 
                                                Integer index = 1);
                                                
   // Methods used to break apart lines of script
   void                          ChunkLine(void);
   bool                          IsGroup(const char *text);
   Integer                       SkipWhiteSpace(Integer start = 0, 
                                                const std::string &text = "");
   void                          WriteParameterValue(GmatBase *obj, Integer id);
   Integer                       FindDelimiter(const std::string &str,
                                               const std::string &specChar = "");
   std::string                   GetToken(const std::string &tokstr = "");

// temporarily make public to test this piece
public:
   // Handlers for specific constructs
   StringArray&                  Decompose(const std::string &chunk);

protected:
   StringArray&                  SeparateSpaces(const std::string &chunk);
   StringArray&                  SeparateDots(const std::string &chunk, 
                                              const std::string &delimiter = ".");
   StringArray&                  SeparateBraces(const std::string &chunk);
   StringArray&                  SeparateBrackets(const std::string &chunk);
   StringArray&                  SeparateParens(const std::string &chunk);
    
   GmatBase*                     FindObject(std::string objName);
   GmatBase*                     FindOwnedObject(StringArray TokenList,
                                                 GmatBase *owner, 
                                                 Integer index);
   bool                          SetParameter(GmatBase *obj, Integer id,
                                              std::string value);
                                                 
   void                          RegisterAliases(void);
   bool                          ConfigureForce(ForceModel *obj, 
                                                std::string& objParm, 
                                                std::string& parm);
};

#endif // INTERPRETER_HPP
