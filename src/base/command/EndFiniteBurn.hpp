//$Header$
//------------------------------------------------------------------------------
//                            EndFiniteBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/01/04
//
/**
 *  Definition code for the EndFiniteBurn command.
 */
//------------------------------------------------------------------------------

#ifndef EndFiniteBurn_hpp
#define EndFiniteBurn_hpp


#include "Command.hpp"
#include "FiniteBurn.hpp"
#include "Spacecraft.hpp"
#include "Thruster.hpp"
#include "FiniteThrust.hpp"


/**
 * EndFiniteBurn Class, used to turn off thrusters used in a finite burn.
 */
class GMAT_API EndFiniteBurn : public GmatCommand
{
public:
	EndFiniteBurn();
	virtual ~EndFiniteBurn();
   EndFiniteBurn(const EndFiniteBurn& endman);
   EndFiniteBurn&         operator=(const EndFiniteBurn& endman);

   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                        const std::string &name);

   virtual GmatBase*    Clone() const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName);

   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   
   virtual bool         Initialize();
   virtual bool         Execute();

protected:
   /// Name of the FiniteBurn object used to set the maneuver details
   std::string          thrustName;
   /// The FiniteThrust that is available for the force models
   FiniteThrust         *burnForce;

   /// Name of the FiniteBurn object used to set the maneuver details
   std::string          burnName;
   /// The FiniteBurn object
   FiniteBurn           *maneuver;
   /// The vector of forces managed by the Sandbox
   std::vector<PhysicalModel*> 
                        *transientForces;
   /// The names of the spacecraft that get maneuvered
   StringArray          satNames;
   /// The spacecraft that get maneuvered
   std::vector<Spacecraft *>
                        sats;
   /// The thrusters that get deactivated
   std::vector<Thruster *>
                        thrusters;
};

#endif // EndFiniteBurn_hpp
