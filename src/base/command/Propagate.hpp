//$Header$
//------------------------------------------------------------------------------
//                                 Propagate
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
 * Definition for the Propagate command class
 */
//------------------------------------------------------------------------------


#ifndef Propagate_hpp
#define Propagate_hpp

#include "GmatCommand.hpp"
#include <list>


// Headers for the referenced classes
#include "Spacecraft.hpp"
#include "Formation.hpp"
#include "PropSetup.hpp"
#include "StopCondition.hpp"


class GMAT_API Propagate : public GmatCommand
{
public:
   Propagate();
   virtual ~Propagate();
   Propagate(const Propagate &prp);
   Propagate&          operator=(const Propagate &prp);

   // Methods used for configuration
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                            const std::string &name);
   
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);
   
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
   
   virtual bool        SetObject(const std::string &name,
                                 const Gmat::ObjectType type,
                                 const std::string &associate = "",
                                 const Gmat::ObjectType associateType =
                                 Gmat::UNKNOWN_OBJECT);
   virtual bool        SetObject(GmatBase *obj, const Gmat::ObjectType type);
   virtual GmatBase*   GetObject(const Gmat::ObjectType type, 
                                 const std::string objName = "");
   virtual void        ClearObject(const Gmat::ObjectType type);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
   virtual const std::string&
                       GetGeneratingString(Gmat::WriteMode mode =
                                           Gmat::SCRIPTING,
                                           const std::string &prefix = "",
                                           const std::string &useName = "");

   // Reference object accessor methods
   virtual GmatBase*   GetRefObject(const Gmat::ObjectType type,
                                    const std::string &name,
                                    const Integer index);
   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name,
                                    const Integer index);
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   
   // Parameter accessor methods
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
    
   virtual Integer     GetIntegerParameter(const Integer id) const;
   virtual Integer     SetIntegerParameter(const Integer id,
                                           const Integer value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value,
                                          const Integer index);
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id, 
                                               const Integer index) const;
   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual bool        SetBooleanParameter(const Integer id,
                                           const bool value);
   virtual bool        GetBooleanParameter(const std::string &label) const;
   virtual bool        SetBooleanParameter(const std::string &label,
                                           const bool value);
 
   virtual bool        TakeAction(const std::string &action,  
                                  const std::string &actionData = "");
    
   // Methods used to run the command
   virtual bool        InterpretAction();
    
   virtual void        SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual bool        Initialize();
   virtual void        FillFormation(SpaceObject *so, StringArray& owners,
                                     StringArray& elements);
   virtual GmatCommand* GetNext();
   virtual bool        Execute();
   virtual void        RunComplete();

protected:
   /// Name of the propagator setup(s) used in this command
   StringArray             propName;
   /// Step direction multipliers used to switch btwn forwards & backwards prop
   Real                    direction;
   /// The (1 or more) spacecraft associated with this propagation, grouped by
   /// propagator
   std::vector<StringArray *>  
                           satName;
   /// Flag used to determine if the spacecraft are propagated coupled
   std::string             currentPropMode;
   /// Frequency used to check for user interrupts of the run
   Integer                 interruptCheckFrequency;
   /// Flag that specifies if we are rejoining a run in progress
   bool                    inProgress;
   /// Flag indicating if this command was already fired once before
   bool                    hasFired;
   /// ID for the spacecraft epoch parameter
   Integer                 epochID;
   /// Starting epoch for the propagation
   std::vector<Real>       baseEpoch;

   /// The propagator(s) used by this command
   std::vector<PropSetup*> prop;
   /// The spacecraft and formations that are propagated
   std::vector<SpaceObject *> sats;
   /// The stopping conditions
   std::vector<StopCondition *> stopWhen;
   /// The time step that we need to unterpolate across
   Real                    stopInterval;
   /// Index to the Stopcondition that triggered the stop
   Integer                 stopTrigger;
   /// Names of the spacecraft used in the stopping conditions
   StringArray             stopSatNames;
   /// The spacecraft used by the stopping conditions
   std::vector<SpaceObject *> stopSats;
   /// Stopping condition evaluation requires propagation; the satBuffer lets us
   /// restore the Spacecraft and Formations to the state needed for the last 
   /// step 
   std::vector<Spacecraft *> satBuffer;
   std::vector<Formation *>  formBuffer;
   /// The object array used in GetRefObjectArray()
   ObjectArray             objectArray;

   // Parameters moved from Execute so that it can be reentrant
   /// Time elapsed during this Propagate
   RealArray               elapsedTime;
   /// Start epoch for the step
   RealArray               currEpoch;
   /// The Propagator
   std::vector<Propagator*> p;
   /// The ForceModel
   std::vector<ForceModel*> fm;
   
   // Members used to flag most recent detected stop, so we don't stop multiple 
   // times at the same point
   bool                     hasStoppedOnce;
   Integer                  stepsTaken;
   

   /// Allowed modes of propagation
   enum PropModes
   {
      INDEPENDENT,
      SYNCHRONIZED,
      BACK_PROP,
      PropModeCount
   };

   /// The state that is propagated
   Real                    *state;
   /// Data sent to the Publisher
   Real                    *pubdata;
   /// Flag for stopping
   bool                    stopCondMet;
   /// Epoch used for stop
   Real                    stopEpoch;
   /// Dimension used for (local) state vector
   Integer                 dim;
   /// Identifies when the command is in single step mode
   bool                    singleStepMode;
   /// List of forces that can be turned on or off by other commands
   std::vector<PhysicalModel*> 
                           *transientForces;
   /// Variable that tracks the current propagation mode
   PropModes               currentMode;
   
   // Stopping condition parameter IDs
   /// Epoch on the stopping condition
   Integer                 stopCondEpochID;
   /// Epoch on the stopping condition
   Integer                 stopCondBaseEpochID;
   /// Epoch on the stopping condition
   Integer                 stopCondStopVarID;

   /// Parameter IDs
   enum
   {
      AVAILABLE_PROP_MODES = GmatCommandParamCount,
      PROP_COUPLED,
      INTERRUPT_FREQUENCY,
      SAT_NAME,
      PROP_NAME,
      STOP_WHEN,
      PROP_FORWARD,
      PropagateCommandParamCount,
   };

   /// Parameter ID Types
   static const Gmat::ParameterType
      PARAMETER_TYPE[PropagateCommandParamCount - GmatCommandParamCount];
   /// Parameter ID strings
   static const std::string
      PARAMETER_TEXT[PropagateCommandParamCount - GmatCommandParamCount];
   /// Array of allowed propagation modes
   static std::string      PropModeList[PropModeCount];

   
   virtual void            SetNames(const std::string& name, 
                                    StringArray& owners, StringArray& elements);
   virtual void            CheckForOptions(Integer &loc, 
                                           std::string& generatingString);

   virtual void            AssemblePropagators(Integer &loc, 
                              std::string& generatingString);
   void                    FindSetupsAndStops(Integer &loc, 
                              std::string& generatingString,
                              StringArray &setupStrings, 
                              StringArray &stopStrings);
   void                    ConfigurePropSetup(std::string &setupDesc);
   void                    ConfigureStoppingCondition(std::string &stopDesc);
   void                    CleanString(std::string &theString, 
                              const StringArray *extras = NULL);

//   bool                    InterpretParameter(const std::string text, 
//                                              std::string &paramType, 
//                                              std::string &paramObj, 
//                                              std::string &parmSystem);
   virtual void            PrepareToPropagate();
   virtual void            CheckStopConditions(Integer EpochID);
   virtual void            TakeFinalStep(Integer EpochID, Integer trigger);
   
   virtual bool            TakeAStep(Real propStep = 0.0);
   
   
   void                    AddTransientForce(StringArray *sats, ForceModel *p);
   void                    ClearTransientForces();
   std::string             CreateParameter(const std::string &name);
   
   void                    AddToBuffer(SpaceObject *so);
   void                    EmptyBuffer();
   void                    BufferSatelliteStates(bool fillingBuffer = true);
   bool                    CheckFirstStepStop();
   
private:
    
};


#endif // Propagate_hpp
