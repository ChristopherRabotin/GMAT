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
 * Class definition for the Interpreter base class
 */
//------------------------------------------------------------------------------


#ifndef Interpreter_hpp
#define Interpreter_hpp

#include <map>
#include <list>
#include <iostream>
#include "gmatdefs.hpp"
#include "InterpreterException.hpp"
#include "GmatBase.hpp"
#include "TextParser.hpp"
#include "ScriptReadWriter.hpp"
#include "ElementWrapper.hpp"

// Forward references for GMAT core objects
class Spacecraft;
class Formation;
class Hardware;
class Propagator;
class ForceModel;
class PropSetup;
class PhysicalModel;
class SolarSystem;
class CelestialBody;
class Parameter;
class GmatCommand;
class CoordinateSystem;
class AxisSystem;
//class Publisher;
class Subscriber;
class Burn;


class Moderator;


/**
 * Interpreter is the base class for the GMAT Interpreter subsystem.  
 * 
 * Interpreter defines the interfaces used to parse the text files that control 
 * execution in GMAT.  It also provides the interfaces to write text files out, 
 * either to the file system, the screen, or the GUI.
 */
class GMAT_API Interpreter
{
public:
   Interpreter();
   virtual ~Interpreter();
   
   //------------------------------------------------------------------------------
   // bool Interpret()
   //------------------------------------------------------------------------------
   /**
    * Retrieves text input from a stream and translate it into GMAT objects and
    * actions.
    * 
    * This method gets overridden by derived classes.
    * 
    * @return true on success, false on failure.
    */
   //------------------------------------------------------------------------------
   virtual bool Interpret() = 0;
   
   //------------------------------------------------------------------------------
   // bool Build()
   //------------------------------------------------------------------------------
   /**
    * Accesses GMAT objects and actions and writes them to a stream.
    * 
    * This method gets overridden by derived classes.
    * 
    * @return true on success, false on failure.
    */
   //------------------------------------------------------------------------------
   virtual bool Build(Gmat::WriteMode mode) = 0;
   
   virtual Parameter* CreateSystemParameter(const std::string &str);
   virtual Parameter* CreateParameter(const std::string &type,
                                      const std::string &name,
                                      const std::string &ownerName = "",
                                      const std::string &depName = "");
   
   GmatCommand* InterpretGMATFunction(const std::string &pathAndName);
   
   const StringArray& GetListOfObjects(Gmat::ObjectType type);
   GmatBase* GetConfiguredObject(const std::string &name);
   GmatBase* CreateObject(const std::string &type, const std::string &name);
   SolarSystem* GetSolarSystemInUse();
   
   const StringArray& GetErrorList() { return errorList; }
   void SetHeaderComment(const std::string &comment){headerComment = comment;}
   void SetFooterComment(const std::string &comment){footerComment = comment;}
   
   // to check commands
   bool ValidateCommand(GmatCommand *cmd);
   // to check subscriber
   bool ValidateSubscriber(GmatBase *obj);
   
protected:
   
   // subclasses accessable member data

   /// A pointer to the ScriptReadWriter used when reading or writing script.
   ScriptReadWriter* theReadWriter;
   Moderator    *theModerator;
   TextParser   theTextParser;
   bool         inCommandMode;
   bool         inRealCommandMode;
   bool         initialized;
   bool         parsingDelayedBlock;
   bool         ignoreError;
   
   /// For handling delayed blocks
   StringArray  delayedBlocks;
   StringArray  delayedBlockLineNumbers;
   
   /// Block type and comments
   std::string  headerComment;
   std::string  footerComment;
   std::string  currentBlock;
   std::string  currentLine;
   std::string  lineNumber;
   Gmat::BlockType currentBlockType;
   
   /// Error handling data
   bool continueOnError;
   std::string errorMsg1;
   std::string errorMsg2;
   StringArray errorList;
   
   void Initialize();
   void RegisterAliases();
   
   bool FindPropertyID(GmatBase *obj, const std::string &chunk, GmatBase **owner,
                       Integer &id, Gmat::ParameterType &type);
   
   GmatBase* FindObject(const std::string &name, 
                        const std::string &ofType = "");
   
   Parameter* CreateArray( const std::string &arrayStr);   
   Parameter* GetArrayIndex(const std::string &arrayStr,
                            Integer &row, Integer &col);
   
   AxisSystem* CreateAxisSystem(std::string type, GmatBase *owner);
   
   // for commands
   GmatCommand* CreateCommand(const std::string &type, const std::string &desc,
                              bool &retFlag, GmatCommand *inCmd = NULL);
   GmatCommand* AppendCommand(const std::string &type, bool &retFlag,
                              GmatCommand *inCmd = NULL);
   GmatCommand* CreateAssignmentCommand(const std::string &lhs,
                                        const std::string &rhs, bool &retFlag,
                                        GmatCommand *inCmd = NULL);
   
   bool AssembleCommand(GmatCommand *cmd, const std::string &desc);
   bool AssembleCallFunctionCommand(GmatCommand *cmd, const std::string &desc);
   bool AssembleConditionalCommand(GmatCommand *cmd, const std::string &desc);
   bool AssembleForCommand(GmatCommand *cmd, const std::string &desc);
   bool AssembleGeneralCommand(GmatCommand *cmd, const std::string &desc);
   
   // for assignment
   GmatBase* MakeAssignment(const std::string &lhs, const std::string &rhs);
   
   // for setting whole object
   bool SetObjectToObject(GmatBase *toObj, GmatBase *fromObj);
   bool SetPropertyToObject(GmatBase *toObj, GmatBase *fromOwner,
                            const std::string &fromProp);
   bool SetArrayToObject(GmatBase *toObj, const std::string &fromArray);
   bool SetValueToObject(GmatBase *toObj, const std::string &value);
   
   // for setting property
   bool SetObjectToProperty(GmatBase *toOwner, const std::string &toProp,
                            GmatBase *fromObj);
   bool SetPropertyToProperty(GmatBase *toOwner, const std::string &toProp,
                              GmatBase *fromOwner, const std::string &fromProp);
   bool SetArrayToProperty(GmatBase *toOwner, const std::string &toProp,
                           const std::string &fromArray);
   bool SetValueToProperty(GmatBase *toOwner, const std::string &toProp,
                           const std::string &value);
   
   // for setting array
   bool SetObjectToArray(GmatBase *toArrObj, const std::string &toArray,
                         GmatBase *fromObj);
   bool SetPropertyToArray(GmatBase *toArrObj, const std::string &toArray,
                           GmatBase *fromOwner, const std::string &fromProp);
   bool SetArrayToArray(GmatBase *toArrObj, const std::string &toArray,
                        GmatBase *fromArrObj, const std::string &fromArray);
   bool SetValueToArray(GmatBase *toArrObj, const std::string &toArray,
                        const std::string &value);
   
   // for setting/getting property value
   bool SetPropertyValue(GmatBase *obj, const Integer id,
                         const Gmat::ParameterType type,
                         const std::string &value,
                         const Integer index = -1);
   
   std::string GetPropertyValue(GmatBase *obj, const Integer id);
   
   bool SetProperty(GmatBase *obj, const Integer id,
                    const Gmat::ParameterType type, const std::string &value);
   
   bool SetComplexProperty(GmatBase *obj, const std::string &prop,
                           const std::string &value);
   bool SetForceModelProperty(GmatBase *obj, const std::string &prop,
                              const std::string &value, GmatBase *fromObj);
   bool SetSolarSystemProperty(GmatBase *obj, const std::string &prop,
                               const std::string &value);
   
   bool FindOwnedObject(GmatBase *owner, const std::string toProp,
                        GmatBase **ownedObj, Integer &id, Gmat::ParameterType &type);
   
   // for setting/getting array value
   Real GetArrayValue(const std::string &arrayStr, Integer &row, Integer &col);
   
   bool IsArrayElement(const std::string &str);
   
   // for error handling
   void HandleError(BaseException &e, bool writeLine = true, bool warning = false);
   void HandleErrorMessage(BaseException &e, const std::string &lineNumber,
                           const std::string &line, bool writeLine = true,
                           bool warning = false);
   
   // for branch command checking
   bool IsBranchCommand(const std::string &str);
   bool CheckBranchCommands(const IntegerArray &lineNumbers,
                            const StringArray &lines);
   
   // Final setting of reference object pointers needed by the GUI
   bool FinalPass();
   
   // for handling wrappers 
      
private:
   
   // Mapping between the object base class strings and the objecttypes enum
   // It is not used
   //std::map<std::string, Integer>  typeMap;
   
   StringArray   commandList;
   StringArray   atmosphereList;
   StringArray   attitudeList;
   StringArray   axisSystemList;
   StringArray   burnList;
   StringArray   calculatedPointList;
   StringArray   functionList;
   StringArray   hardwareList;
   StringArray   parameterList;
   StringArray   physicalModelList;
   StringArray   propagatorList;
   StringArray   solverList;
   StringArray   stopcondList;
   StringArray   subscriberList;
   
   bool IsObjectType(const std::string &type);
   bool IsParameterType(const std::string &desc);
   bool CheckForSpecialCase(GmatBase *obj, Integer id, std::string &value);
   bool CheckUndefinedReference(GmatBase *obj, bool writeLine = true);
   bool SetCommandParameter(GmatCommand *cmd, const std::string &param,
                            const std::string &msg, bool isNumberAllowed,
                            bool isArrayAllowed);
   void WriteParts(const std::string &title, StringArray &parts);
   
   // for wrappers
   ElementWrapper* CreateElementWrapper(const std::string &desc,
                                        bool parametersFirst = false);
   void CreateParameterWrapper(Parameter *param, ElementWrapper **ew,
                               Gmat::WrapperDataType &itsType);
   
};

#endif // INTERPRETER_HPP
