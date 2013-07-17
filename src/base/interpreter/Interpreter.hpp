//$Id$
//------------------------------------------------------------------------------
//                                  Interpreter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
class ODEModel;
class PropSetup;
class PhysicalModel;
class SolarSystem;
class CelestialBody;
class Parameter;
class GmatCommand;
class CoordinateSystem;
class AxisSystem;
class Subscriber;
class Burn;
class Function;
class Moderator;
class Validator;
class Interface;


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
   Interpreter(SolarSystem *ss = NULL, ObjectMap *objMap = NULL);
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
   
   const StringArray& GetListOfObjects(Gmat::ObjectType type);
   const StringArray& GetListOfObjects(const std::string &typeName);
   const StringArray& GetListOfViewableSubtypesOf(Gmat::ObjectType type);
   const StringArray& GetListOfViewableCommands();
   
   GmatBase* GetConfiguredObject(const std::string &name);
   GmatBase* FindObject(const std::string &name, const std::string &ofType = "");
   GmatBase* CreateObject(const std::string &type, const std::string &name,
                          Integer manage = 1, bool createDefault = false,
                          bool includeLineOnError = true);
   
   void SetConfiguredObjectMap();
   void SetSolarSystemInUse(SolarSystem *ss);
   SolarSystem* GetSolarSystemInUse();
   void SetObjectMap(ObjectMap *objMap, bool forFunction = false);
   ObjectMap* GetObjectMap();
   void SetFunction(Function *func);
   Function* GetFunction();
   
   void SetContinueOnError(bool flag);
   bool GetContinueOnError();
   bool IsInCommandMode();
   bool FoundBeginMissionSequence();
   
   const StringArray& GetErrorList() { return errorList; }
   void SetHeaderComment(const std::string &comment){headerComment = comment;}
   void SetFooterComment(const std::string &comment){footerComment = comment;}
   
   bool IsObjectType(const std::string &type);
   bool IsCommandType(const std::string &type);
   
   Gmat::ObjectType GetObjectType(const std::string &type);
   
   // to check commands
   bool ValidateCommand(GmatCommand *cmd);
   // to create subscriber wrappers
   bool ValidateSubscriber(GmatBase *obj);
   
   bool SetForceModelProperty(GmatBase *obj, const std::string &prop,
                              const std::string &value, GmatBase *fromObj);
   bool SetDragForceProperty(GmatBase *obj, const std::string &pmType,
                             const std::string &pmField, const std::string &value);
   bool SetMeasurementModelProperty(GmatBase *obj, const std::string &prop,
                              const std::string &value);
   bool SetTrackingDataProperty(GmatBase *obj, const std::string &prop,
                              const std::string &value);
   bool SetTrackingSystemProperty(GmatBase *obj, const std::string &prop,
                              const std::string &value);
   bool SetDataStreamProperty(GmatBase *obj, const std::string &property,
                              const std::string &value);
   bool FindOwnedObject(GmatBase *owner, const std::string toProp,
                        GmatBase **ownedObj, Integer &id, Gmat::ParameterType &type);
   
   bool FindPropertyID(GmatBase *obj, const std::string &chunk, GmatBase **owner,
                       Integer &id, Gmat::ParameterType &type);
   
   void BuildCreatableObjectMaps();
   StringArray GetCreatableList(Gmat::ObjectType type,
                                const std::string subType = "");
   
   virtual void SetInputFocus();
   virtual void NotifyRunCompleted();
   virtual void UpdateView(Integer type = 7);
   virtual void CloseCurrentProject();
   virtual void StartMatlabServer();
   
   Interface* GetMatlabInterface();
   bool OpenMatlabEngine();
   bool CloseMatlabEngine();   
   
protected:
   
   Moderator    *theModerator;
   SolarSystem  *theSolarSystem;
   Validator    *theValidator;
   
   // Object map to be used for finding objects
   ObjectMap    *theObjectMap;
   // String array to be used for finding temporary object names
   StringArray  tempObjectNames;
   
   /// A pointer to the ScriptReadWriter used when reading or writing script.
   ScriptReadWriter  *theReadWriter;
   TextParser        theTextParser;
   
   bool         inCommandMode;
   bool         inRealCommandMode;
   bool         beginMissionSeqFound;
   bool         initialized;
   bool         parsingDelayedBlock;
   bool         ignoreError;
   bool         inScriptEvent;
   
   /// For handling GmatFunction
   bool         gmatFunctionsAvailable;
   bool         inFunctionMode;
   bool         hasFunctionDefinition;
   Function     *currentFunction;
   
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
   bool        continueOnError;
   std::string errorMsg1;
   std::string errorMsg2;
   std::string debugMsg;
   StringArray errorList;
   
   void Initialize();
   void RegisterAliases();
   
   Parameter* GetArrayIndex(const std::string &arrayStr,
                            Integer &row, Integer &col);
   
   AxisSystem* CreateAxisSystem(std::string type, GmatBase *owner);
   
   // for commands
   bool         ParseAndSetCommandName(GmatCommand *cmd, const std::string &cmdType,
                                       const std::string &desc, std::string &newDesc);
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
   bool AssembleTargetCommand(GmatCommand *cmd, const std::string &desc);
   bool AssembleOptimizeCommand(GmatCommand *cmd, const std::string &desc);
   bool AssembleFiniteBurnCommand(GmatCommand *cmd, const std::string &desc);
   bool AssembleReportCommand(GmatCommand *cmd, const std::string &desc);
   bool AssembleCreateCommand(GmatCommand *cmd, const std::string &desc);
   bool SetCommandRefObjects(GmatCommand *cmd, const std::string &desc);
   
   // for assignment
   GmatBase* MakeAssignment(const std::string &lhs, const std::string &rhs);
   
   // for setting whole object
   bool SetObjectToObject(GmatBase *toObj, GmatBase *fromObj, const std::string &rhs);
   bool SetObjectToProperty(GmatBase *toObj, GmatBase *fromOwner,
                            const std::string &fromProp);
   bool SetObjectToArray(GmatBase *toObj, const std::string &fromArray);
   bool SetObjectToValue(GmatBase *toObj, const std::string &value);
   
   // for setting property
   bool SetPropertyToObject(GmatBase *toOwner, const std::string &toProp,
                            GmatBase *fromObj);
   bool SetPropertyToProperty(GmatBase *toOwner, const std::string &toProp,
                              GmatBase *fromOwner, const std::string &fromProp);
   bool SetPropertyToArray(GmatBase *toOwner, const std::string &toProp,
                           const std::string &fromArray);
   bool SetPropertyToValue(GmatBase *toOwner, const std::string &toProp,
                           const std::string &value);
   
   // for setting array
   bool SetArrayToObject(GmatBase *toArrObj, const std::string &toArray,
                         GmatBase *fromObj);
   bool SetArrayToProperty(GmatBase *toArrObj, const std::string &toArray,
                           GmatBase *fromOwner, const std::string &fromProp);
   bool SetArrayToArray(GmatBase *toArrObj, const std::string &toArray,
                        GmatBase *fromArrObj, const std::string &fromArray);
   bool SetArrayToValue(GmatBase *toArrObj, const std::string &toArray,
                        const std::string &value);
   
   // for setting/getting property value
   bool SetPropertyValue(GmatBase *obj, const Integer id,
                         const Gmat::ParameterType type,
                         const std::string &value,
                         const Integer index = -1, const Integer colIndex = -1);
   bool SetPropertyObjectValue(GmatBase *obj, const Integer id,
                               const Gmat::ParameterType type,
                               const std::string &value,
                               const Integer index = -1);
   bool SetPropertyStringValue(GmatBase *obj, const Integer id,
                               const Gmat::ParameterType type,
                               const std::string &value,
                               const Integer index = -1);
   
   std::string GetPropertyValue(GmatBase *obj, const Integer id);
   
   bool SetProperty(GmatBase *obj, const Integer id,
                    const Gmat::ParameterType type, const std::string &value);
   
   bool SetComplexProperty(GmatBase *obj, const std::string &prop,
                           const std::string &value);
   bool SetSolarSystemProperty(GmatBase *obj, const std::string &prop,
                               const std::string &value);
   
   // for setting/getting array value
   Real GetArrayValue(const std::string &arrayStr, Integer &row, Integer &col);
   bool IsArrayElement(const std::string &str);
   
   // for Variable expression
   bool ParseVariableExpression(Parameter *var, const std::string &exp);
   
   // for error handling
   void HandleError(const BaseException &e, bool writeLine = true, bool warning = false);
   void HandleErrorMessage(const BaseException &e, const std::string &lineNumber,
                           const std::string &line, bool writeLine = true,
                           bool warning = false);
   
   // for branch command checking
   bool IsBranchCommand(const std::string &str);
   bool CheckBranchCommands(const IntegerArray &lineNumbers,
                            const StringArray &lines);
   
   // for setting object inside branch command
   void SetObjectInBranchCommand(GmatCommand *brCmd, const std::string &branchType,
                                 const std::string &childType,
                                 const std::string &objName);
   
   // Final setting of reference object pointers needed by the GUI and validation
   bool FinalPass();
   bool FinalPassSubscribers();
   
   // for debug
   void WriteStringArray(const std::string &title1, const std::string &title2,
                         const StringArray &parts);
   void WriteForceModel(GmatBase *obj);
   
   // for GamtFunction handling
   bool CheckFunctionDefinition(const std::string &funcPathAndName,
                                GmatBase *function, bool fullCheck = true);
   bool BuildFunctionDefinition(const std::string &str);
   void ClearTempObjectNames();
   
   bool ValidateMcsCommands(GmatCommand *first, GmatCommand *parent = NULL,
         StringArray *missingObjects = NULL, std::string
         *accumulatedErrors = NULL);

   bool ValidateSolverCmdLevels(GmatCommand *bc, Integer cmdLevel);

private:
      
   StringArray   commandList;
   StringArray   atmosphereList;
   StringArray   attitudeList;
   StringArray   axisSystemList;
   StringArray   burnList;
   StringArray   calculatedPointList;
   StringArray   dataFileList;
   StringArray   ephemFileList;
   StringArray   functionList;
   StringArray   hardwareList;
   StringArray   measurementList;
   StringArray   trackingSystemList;
   StringArray   obtypeList;
   StringArray   odeModelList;
   StringArray   parameterList;
   StringArray   physicalModelList;
   StringArray   propagatorList;
   StringArray   solverList;
   StringArray   stopcondList;
   StringArray   subscriberList;
   StringArray   spacePointList;
   StringArray   celestialBodyList;
   StringArray   eventLocatorList;
   StringArray   interfaceList;
   
   StringArray   matlabFunctionNames;
   
   static StringArray   allObjectTypeList;
   static StringArray   viewableCommandList;
   static std::map<std::string, Gmat::ObjectType> objectTypeMap;
   bool IsParameterType(const std::string &desc);
   bool CheckForSpecialCase(GmatBase *obj, Integer id, std::string &value);
   bool CheckUndefinedReference(GmatBase *obj, bool writeLine = true);
   bool HandleMathTree(GmatCommand *cmd);
   bool HasFilenameTypeParameter(GmatCommand *cmd);
   
   static const std::string defaultIndicator;
};

#endif // INTERPRETER_HPP
