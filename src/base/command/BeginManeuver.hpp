//$Header$
//------------------------------------------------------------------------------
//                            BeginManeuver
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
 *  Definition code for the BeginManeuver command.
 */
//------------------------------------------------------------------------------

#ifndef BeginManeuver_hpp
#define BeginManeuver_hpp


#include "Command.hpp"
#include "FiniteBurn.hpp"
#include "Spacecraft.hpp"
#include "Thruster.hpp"


/**
 * BeginManeuver Class, used to turn on thrusters used in a finite burn.
 */
class GMAT_API BeginManeuver : public GmatCommand
{
public:
   BeginManeuver();
   virtual ~BeginManeuver();
   BeginManeuver(const BeginManeuver& begman);
   BeginManeuver&       operator=(const BeginManeuver& begman);
   
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
   /// The thrusters that get activated
   std::vector<Thruster *>
                        thrusters;
};

#endif // BeginManeuver_hpp
