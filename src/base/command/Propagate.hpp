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
   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual bool        SetBooleanParameter(const Integer id,
                                           const bool value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id) const; 

   // Methods used to run the command
   virtual bool        InterpretAction();
    
   virtual bool        Initialize();
   virtual void        FillFormation(SpaceObject *so, StringArray& owners,
                                     StringArray& elements);
   virtual GmatCommand* GetNext();
   virtual bool        Execute();

protected:
   // We may eventually want to make this a list of propagator names for the
   // propagators driven by this command, like this:
   //    /// List of the names of propagator setups used in this command
   //    std::vector<std::string>  propName;

   /// Name of the propagator setup used in this command
   std::string             propName;
   /// The (1 or more) spacecraft associated with this propagation
   StringArray             satName;
   /// Flag used to determine if the spacecraft are propagated coupled
   bool                    propCoupled;
   /// Frequency used to check for user interrupts of the run
   Integer                 interruptCheckFrequency;
   /// Flag that specifies if we are rejoining a run in progress
   bool                    inProgress;
   /// Starting epoch for the propagation
   Real                    baseEpoch;

   // We may eventually want to make this a list of propagators all driven by
   // this command, like this:
   //    /// List of the propagator setups used in this command
   //    std::vector<Propagator>  prop;
    
   /// The propagator used by this command
   PropSetup               *prop;      // Likely to change when the propagator
   // infrastructure is coded -- might
   // just be a Propagator *
   /// The spacecraft that are propagated
   std::vector<SpaceObject *> sats;
   /// The stopping conditions
   std::vector<StopCondition *> stopWhen;
   /// The object array used in GetRefObjectArray()
   ObjectArray objectArray;
   
   // For convenience, set variables for the parameter IDs
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
   Real                    elapsedTime;
   /// Start epoch for the step
   Real                    currEpoch;
   /// The Propagator
   Propagator              *p;
   /// The ForceModel
   ForceModel              *fm;


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
   
   virtual void            SetNames(const std::string& name, 
                                    StringArray& owners, StringArray& elements);
   

public:    
   // Accessors (Temporary, to support internal prop duration)
   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
    
private:
    
};


#endif // PROPAGATE_HPP
