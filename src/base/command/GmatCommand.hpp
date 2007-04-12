//$Header$
//------------------------------------------------------------------------------
//                                  GmatCommand
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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
#include <algorithm>        // for find()
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CommandException.hpp"

// Headers used by commands that override InterpretAction
#include "StringUtil.hpp"
#include "TextParser.hpp"
#include "ElementWrapper.hpp"

// Headers for the referenced classes
#include "SolarSystem.hpp"   // for SolarSystem
#include "Publisher.hpp"     // For the Publisher and ...
#include "Subscriber.hpp"    // ... base of the Subscribers
#include "SpaceObject.hpp"   // for SpaceObjects

// Forward reference for the transient force vector
class PhysicalModel;


/**
 * GMAT GmatCommand Base Class, used for timeline elements in the script
 *
 * The GMAT GmatCommands all follow a "late-binding" philosophy, in that they do
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
           
   // other methods for setting up the object w.r.t the elements needed
   virtual const StringArray& 
                        GetWrapperObjectNameArray();
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                        const std::string &withName);
   virtual void         ClearWrappers();
   
   // Methods used to setup objects
   virtual bool         SetObject(const std::string &name,
                                  const Gmat::ObjectType type,
                                  const std::string &associate = "",
                                  const Gmat::ObjectType associateType =
                                       Gmat::UNKNOWN_OBJECT);
   virtual bool         SetObject(GmatBase *obj,
                                  const Gmat::ObjectType type);
   virtual GmatBase*    GetObject(const Gmat::ObjectType type, 
                                  const std::string objName = "");
   
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   
   virtual void         SetPublisher(Publisher *p);
   
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
   
   bool                HasConfigurationChanged();
   virtual void        ConfigurationChanged(bool tf, bool setAll = false);

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
   
   
   /// Map containing names and associated types
   std::map<std::string, Gmat::ObjectType> 
                        association;
   /// List of the associated objects
   StringArray          objects;
   /// Flag used to determine if associations have been made
   bool                 initialized;
   /// Pointer to the next GmatCommand in the sequence; NULL at the end
   GmatCommand          *next;
   /// Pointer to the previous GmatCommand in the sequence; NULL passed begining
   GmatCommand          *previous;
   /// Indicator of the current nesting level
   Integer              level;
   /// Object store obtained from the Sandbox
   std::map<std::string, GmatBase *>
                        *objectMap;
   /// Solar System, set by the local Sandbox
   SolarSystem          *solarSys;
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
//   /// Optional comment string associated with the command
//   std::string          comment;
   /// Flag indicating that a command has been changed by a user
   bool                 commandChanged;
   
   /// Text parser used by commands that override InterpretAction
   TextParser           parser;
   /// The list of settable entities for the command
   StringArray          settables;
   /// The list of names of Wrapper objects
   StringArray          wrapperObjectNames;
   /// List used to initialize the local TextParser
   StringArray          commandNameList;
      
   virtual bool         AssignObjects();
   virtual bool         ClearObjects();
   virtual void         BuildCommandSummary(bool commandCompleted = true);
   virtual void         BuildCommandSummaryString(bool commandCompleted = true);
   virtual const std::string 
                        BuildMissionSummaryString(const GmatCommand* head = NULL);

   // for Debug (just one copy)
   virtual void         ShowCommand(const std::string &prefix,
                                    const std::string &title1, GmatCommand *cmd1,
                                    const std::string &title2 = "",
                                    GmatCommand *cmd2 = NULL);
   
   // for the Parameters in Commands updates
   StringArray          InterpretPreface();
   bool                 IsSettable(const std::string &setDesc);
   bool                 SeparateEquals(const std::string &description,
                                       std::string &lhs, std::string &rhs);
   
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
   std::vector <SpaceObject*>
                        satVector;
   Integer              satsInSandbox;

   // Temporary -- replace when convenient
   void CartToKep(const Rvector6 in, Rvector6 &out);
   GmatBase* FindObject(const std::string &name);

   
   // Method(s) used for ParametersInCommands
   bool                SetWrapperReferences(ElementWrapper &wrapper);
};

#endif // Command_hpp
