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


#ifndef PROPAGATE_HPP
#define PROPAGATE_HPP

#include "Command.hpp"
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
   Propagate(const Propagate &p);
   Propagate&          operator=(const Propagate &p);

   // Methods used for configuration
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                            const std::string &name);
   
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);

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

   // Reference object accessor methods
   //loj: 6/25/04 added
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
   virtual bool        TakeAction(const std::string &action,  
                                  const std::string &actionData = "");
    
   // Methods used to run the command
   virtual bool        InterpretAction();
    
   virtual bool        Initialize();
   virtual void        FillFormation(SpaceObject *so, StringArray& owners,
                                     StringArray& elements);
   virtual GmatCommand* GetNext();
   virtual bool        Execute();

protected:
   /// Name of the propagator setup(s) used in this command
   StringArray             propName;
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
   /// Starting epoch for the propagation
   std::vector<Real>       baseEpoch;

   /// The propagator(s) used by this command
   std::vector<PropSetup*> prop;
   /// The spacecraft and formations that are propagated
   std::vector<SpaceObject *> sats;
   /// The stopping conditions
   std::vector<StopCondition *> stopWhen;
   /// Names of the spacecraft used in the stopping conditions
   StringArray             stopSatNames;
   /// The spacecraft used by the stopping conditions
   std::vector<SpaceObject *> stopSats;
   /// The object array used in GetRefObjectArray()
   ObjectArray objectArray;
   
   // For convenience, set variables for the parameter IDs
   /// ID for available propagation modes
   const Integer           availablePropModesID;
   /// ID for the boolean flag used for the prop mode
   const Integer           propCoupledID;
   /// ID for the number of iterations before calling to check for interrupts
   const Integer           interruptCheckFrequencyID;
   /// ID for the satellite name array
   const Integer           satNameID;
   /// ID for the propagator name
   const Integer           propNameID;
   /// ID used to get the stopping conditions
   const Integer           stopWhenID;
    
   /// Temporary parameter used to stop on time
   Real                    secondsToProp;
   /// ID for the temporary parameter
   const Integer           secondsToPropID;
   
   // Parameters moved from Execute so that it can be reentrant
   /// Time elapsed during this Propagate
   std::vector<Real>       elapsedTime;
   /// Start epoch for the step
   std::vector<Real>       currEpoch;
   /// The Propagator
   std::vector<Propagator*> p;
   /// The ForceModel
   std::vector<ForceModel*> fm;


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
   
   /// Allowed modes of propagation
   enum PropModes {
                     INDEPENDENT, 
                     SYNCHRONIZED,
                     PROP_MODE_COUNT
   };
   /// Variable that tracks the current propagation mode
   PropModes         currentMode;
   /// Array of allowed propagation modes
   static std::string      PropModeList[PROP_MODE_COUNT];
   
   virtual void            SetNames(const std::string& name, 
                                    StringArray& owners, StringArray& elements);
   virtual void            CheckForOptions(Integer &loc, 
                                           std::string& generatingString);
   virtual void            AssemblePropagators(Integer &loc, 
                                               std::string& generatingString);
   virtual bool            TakeAStep(Real propStep = 0.0);

public:    
   // Accessors (Temporary, to support internal prop duration)
   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
    
private:
    
};


#endif // PROPAGATE_HPP
