//$Header$
//------------------------------------------------------------------------------
//                            EndManeuver
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
 *  Definition code for the EndManeuver command.
 */
//------------------------------------------------------------------------------

#ifndef EndManeuver_hpp
#define EndManeuver_hpp


#include "Command.hpp"
#include "FiniteBurn.hpp"
#include "Spacecraft.hpp"
#include "Thruster.hpp"


/**
 * EndManeuver Class, used to turn off thrusters used in a finite burn.
 */
class GMAT_API EndManeuver : public GmatCommand
{
public:
	EndManeuver();
	virtual ~EndManeuver();
   EndManeuver(const EndManeuver& endman);
   EndManeuver&         operator=(const EndManeuver& endman);

   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                        const std::string &name);

   virtual GmatBase*    Clone() const;
   virtual bool         Initialize();
   virtual bool         Execute();

protected:
   /// Name of the FiniteBurn object used to set the maneuver details
   std::string          burnName;
   /// The FiniteBurn object
   FiniteBurn           *maneuver;
   /// The names of the spacecraft that get maneuvered
   StringArray          satNames;
   /// The spacecraft that get maneuvered
   std::vector<Spacecraft *>
                        sats;
   /// The thrusters that get deactivated
   std::vector<Thruster *>
                        thrusters;
};

#endif // EndManeuver_hpp
