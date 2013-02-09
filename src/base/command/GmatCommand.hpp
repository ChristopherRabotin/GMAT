//$Id$
//------------------------------------------------------------------------------
//                                  GmatCommand
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Definition for the GmatCommand base class
 */
//------------------------------------------------------------------------------


#ifndef Command_hpp
#define Command_hpp


#include <map>              // for mapping between object names and types
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CommandException.hpp"

// Headers used by commands that override InterpretAction
#include "TextParser.hpp"
#include "ElementWrapper.hpp"

// Headers for the referenced classes
#include "SolarSystem.hpp"    // For SolarSystem
#include "Publisher.hpp"      // For the Publisher and ...
#include "SpaceObject.hpp"    // For SpaceObjects
#include "TriggerManager.hpp" // Trigger managers, usually from a plugin

// Forward reference for the transient force vector
class PhysicalModel;

// Forward reference for the calling FunctionManager
class FunctionManager;

// Forward reference for the function containing this command
class Function;

// Forward reference for event locators
class EventLocator;

/**
 * GmatCommand Base Class, used for Mission Control Sequence elements in scripts
 *
 * The GmatCommands all follow a "late-binding" philosophy, in that they do
 * not set object associations until the Sandbox has been populated with both
 * the objects that are used in the model and with the complete GmatCommand 
 * sequence.  Once the Sandbox is populated, it initializes the GmatCommand 
 * sequence by calling Initialize() on each GmatCommand, and then runs the
 * sequence by calling Execute() on the first GmatCommand in the sequence.
 */
class GMAT_API GmatCommand : public GmatBase
{
public:
   // class constructor
   GmatCommand(const std::string &typeStr);
   // class destructor
   virtual ~GmatCommand();
   // Copy constructor
   GmatCommand(const GmatCommand &c);
   // Assignment operator
   GmatCommand&         operator=(const GmatCommand &c);
   
   void                 SetGeneratingString(const std::string &gs);
   virtual const std::string&  
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   
   // Methods for function
   virtual void         SetCurrentFunction(Function *function);
   virtual Function*    GetCurrentFunction();
   
   virtual void         SetCallingFunction(FunctionManager *fm);
   
   // other methods for setting up the object w.r.t the elements needed
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                        const std::string &withName);
   virtual void         ClearWrappers();
   virtual void         CheckDataType(ElementWrapper* forWrapper,
                                      Gmat::ParameterType needType,
                                      const std::string &cmdName,
                                      bool ignoreUnsetReference = false);
   
   // Methods used to setup objects
   virtual bool         SetObject(const std::string &name,
                                  const Gmat::ObjectType type,
                                  const std::string &associate = "",
                                  const Gmat::ObjectType associateType =
                                       Gmat::UNKNOWN_OBJECT);
   virtual bool         SetObject(GmatBase *obj,
                                  const Gmat::ObjectType type);
   virtual GmatBase*    GetGmatObject(const Gmat::ObjectType type, 
                                  const std::string objName = "");
   
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetupSummary(const std::string &csName,
                                     bool entireMission = false,
                                     bool physicsOnly = false);
   virtual void         SetSummaryName(const std::string &sumName);
   virtual std::string  GetSummaryName();
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetTriggerManagers(std::vector<TriggerManager*> *trigs);
   virtual void         SetObjectMap(std::map<std::string, GmatBase *> *map);
   virtual ObjectMap*   GetObjectMap();
   virtual void         SetGlobalObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual void         SetEventLocators(std::vector<EventLocator*> *els);
   virtual void         SetPublisher(Publisher *p);
   virtual Publisher*   GetPublisher();
   
   // Methods used in validation
   virtual const StringArray& GetObjectList();
   virtual bool         AcceptsObjectType(Gmat::ObjectType theType);
   virtual bool         Validate();

   // Access methods inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual bool        IsParameterReadOnly(const Integer id) const;
   virtual bool        IsParameterReadOnly(const std::string &label) const;
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value,
                                          const Integer index);
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual std::string GetStringParameter(const std::string &label,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value);
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value,
                                          const Integer index);
   
   // methods to set conditions and operators between multiple conditions;
   // atIndex = -999 means add it to the end of the list; otherwise, add/replace
   // the condition at the specified index
   virtual bool         SetCondition(const std::string &lhs, 
                                     const std::string &operation, 
                                     const std::string &rhs, 
                                     Integer atIndex = -999);
   virtual bool         SetConditionOperator(const std::string &op, 
                                             Integer atIndex = -999);
   
   virtual bool         RemoveCondition(Integer atIndex);
   virtual bool         RemoveConditionOperator(Integer atIndex);
   
   // Sequence methods
   virtual bool         Initialize();
   virtual GmatCommand* GetNext();
   virtual GmatCommand* GetPrevious();
   virtual bool         ForceSetNext(GmatCommand *toCmd);     // dangerous!
   virtual bool         ForceSetPrevious(GmatCommand *toCmd); // dangerous!
   virtual bool         Append(GmatCommand *cmd);
   virtual bool         Insert(GmatCommand *cmd, GmatCommand *prev);
   virtual GmatCommand* Remove(GmatCommand *cmd);
   
   virtual GmatCommand* GetChildCommand(Integer whichOne = 0);
   virtual Integer      GetVariableCount();
   virtual Integer      GetGoalCount();
   
   virtual bool         InterpretAction();
   
   Integer              DepthIncrement();
   bool                 HasPropStateChanged();
   
   //---------------------------------------------------------------------------
   //  bool GmatCommand::Execute()
   //---------------------------------------------------------------------------
   /**
    * The method that is fired to perform the GmatCommand.
    *
    * Derived classes implement this method to perform their actions on
    * GMAT objects.
    *
    * @return true if the GmatCommand runs to completion, false if an error
    *         occurs.
    */
   //---------------------------------------------------------------------------
   virtual bool        Execute() = 0;
   virtual bool        SkipInterrupt();
   virtual void        RunComplete();
   virtual void        SetRunState(Gmat::RunState newState);
   
   bool                HasConfigurationChanged();
   virtual void        ConfigurationChanged(bool tf);
   virtual bool        HasAFunction();
   virtual bool        NeedsServerStartup();
   virtual bool        IsExecuting();
   
   virtual Integer     GetCloneCount();
   virtual GmatBase*   GetClone(Integer cloneIndex = 0);

   virtual bool        AffectsClones();
   virtual GmatBase*   GetUpdatedObject();
   virtual Integer     GetUpdatedObjectParameterIndex();

protected:
   enum
   {
      COMMENT = GmatBaseParamCount,
      SUMMARY,
      MISSION_SUMMARY,
      GmatCommandParamCount
   };
   
   /// Command parameter labels
   static const std::string 
                     PARAMETER_TEXT[GmatCommandParamCount - GmatBaseParamCount];
   /// Command parameter types
   static const Gmat::ParameterType 
                     PARAMETER_TYPE[GmatCommandParamCount - GmatBaseParamCount];
   
   static const Integer MAX_NUM_TANKS;

   static const std::string MISSION_CHANGE_MESSAGE;


   /// Map containing names and associated types
   ObjectTypeMap        association;
   /// List of the associated objects
   StringArray          objects;
   // pointer to the function that contains this command
   Function             *currentFunction;
   // pointer to the function that is calling this command (ignored for all but
   // CallFunction and Assignment)
   FunctionManager      *callingFunction;
   /// Pointer to the next GmatCommand in the sequence; NULL at the end
   GmatCommand          *next;
   /// Pointer to the previous GmatCommand in the sequence; NULL if at the start
   GmatCommand          *previous;
   /// Indicator of the current nesting level
   Integer              level;
   /// Object store obtained from the Sandbox
   ObjectMap            *objectMap;
   /// Object store obtained from the Sandbox
   ////std::map<std::string, GmatBase *> *globalObjectMap;
   ObjectMap            *globalObjectMap;
   /// Solar System, set by the local Sandbox
   SolarSystem          *solarSys;
   /// Trigger managers, set by the local Sandbox
   std::vector<TriggerManager*> *triggerManagers;
   /// Internal coordinate system, set by the local Sandbox
   CoordinateSystem     *internalCoordSys;
   /// transient forces to pass to the function
   std::vector<PhysicalModel *> 
                        *forces;
   /// Event locators used for event detection
   std::vector<EventLocator*>
                        *events;
   /// Publisher for data generated by this GmatCommand
   Publisher            *publisher;
   /// Stream ID issued by the Publisher to identify which Command is publishing
   Integer              streamID;
   /// Change in branch depth caused by this command
   Integer              depthChange;
   /// Flag indicating that the command changes the state vector for propagation
   bool                 commandChangedState;
   /// String used for the command summary data
   std::string          commandSummary;
   /// Coordinate System used for the Command Summary display
   std::string          summaryCoordSysName;
   /// Current coordinate system for Command Summary
   CoordinateSystem     *summaryCoordSys;
   /// flag indicating whether or not the summary for the command is part of
   /// a requested summary for the entire misison (or branch)
   bool                 summaryForEntireMission;
   /// flag indicating whether or not the entire-mission summary should only include
   /// physics-based commands
   bool                 missionPhysicsBasedOnly;
   /// flag indicating whether or not this command is a physics-based command
   bool                 physicsBasedCommand;
   /// flag indicating whether or not to include this type of command in a command summary
   bool                 includeInSummary;
   /// node name (on the GUI) for the command
   std::string          summaryName;

//   /// Optional comment string associated with the command
//   std::string          comment;
   /// Flag indicating that a command has been changed by a user
   bool                 commandChanged;
   
   /// Text parser used by commands that override InterpretAction
   TextParser           parser;
   /// The list of settable entities for the command
   StringArray          settables;
   /// List used to initialize the local TextParser
   StringArray          commandNameList;
   
   /// Count of owned objects created through cloning
   Integer              cloneCount;

   /// Field used for processes that only function in certain run states
   Gmat::RunState       currentRunState;

   // IDs used to buffer the command summary data
   static Integer       satEpochID;
   static Integer       satCdID;
   static Integer       satDragAreaID;
   static Integer       satCrID;
   static Integer       satSRPAreaID;
   static Integer       satTankID;
   static Integer       satThrusterID;
   static Integer       satDryMassID;
   static Integer       satTotalMassID;


   // Command summary data buffers
   Real                 *epochData;
   Real                 *stateData;
   Real                 *parmData;
   Real                 *fuelMassData;
   StringArray          tankNames;

   std::vector <SpaceObject*>
                        satVector;
   Integer              satsInMaps;

   virtual bool         AssignObjects();
   virtual bool         ClearObjects();
   virtual void         BuildCommandSummary(bool commandCompleted = true);
   virtual void         BuildCommandSummaryString(bool commandCompleted = true);
   virtual const std::string 
                        BuildMissionSummaryString(const GmatCommand* head = NULL);
   virtual const std::string
                        BuildNumber(Real value, bool useExp = false, Integer length = 17);
   // for command name
   virtual void         InsertCommandName(std::string &genString);
   
   // for Debug
   virtual void         ShowCommand(const std::string &prefix,
                                    const std::string &title1, GmatCommand *cmd1,
                                    const std::string &title2 = "",
                                    GmatCommand *cmd2 = NULL);
   virtual void         ShowWrapper(const std::string &prefix,
                                    const std::string &title,
                                    ElementWrapper *wrapper);
   virtual void         ShowObjectMaps(const std::string &title = "");
   
   // for the Parameters in Commands updates
   StringArray          InterpretPreface();
   bool                 IsSettable(const std::string &setDesc);
   bool                 SeparateEquals(const std::string &description,
                                       std::string &lhs, std::string &rhs,
                                       bool checkOp = false);
   
   GmatBase* FindObject(const std::string &name);
   
   // Method(s) used for ParametersInCommands
   bool                SetWrapperReferences(ElementWrapper &wrapper);
   
   // Used for deleting old ElementWrappers
   std::vector<ElementWrapper*> oldWrappers;   
   void                ClearOldWrappers();
   void                CollectOldWrappers(ElementWrapper **wrapper);
   void                DeleteOldWrappers();

   // Publish methods that are overridden as needed
   virtual void        PrepareToPublish(bool publishAll = true);
   virtual void        PublishData();

};

#endif // Command_hpp
