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
 *  Implementation code for the BeginManeuver command.
 */
//------------------------------------------------------------------------------

#include "BeginManeuver.hpp"


//------------------------------------------------------------------------------
// BeginManeuver()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
BeginManeuver::BeginManeuver() :
   GmatCommand    ("BeginManeuver")
{
   if (instanceName == "")
      instanceName = "BeginManeuver";
}


//------------------------------------------------------------------------------
// ~BeginManeuver()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BeginManeuver::~BeginManeuver()
{
}


//------------------------------------------------------------------------------
// BeginManeuver(const BeginManeuver& begman)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param begman The command that gets copied.
 */
//------------------------------------------------------------------------------
BeginManeuver::BeginManeuver(const BeginManeuver& begman) :
   GmatCommand       (begman)
{
}


//------------------------------------------------------------------------------
// BeginManeuver& operator=(const BeginManeuver& begman)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param begman The command that gets copied.
 * 
 * @return this instance, with internal data structures set to match the input
 *         instance.
 */
//------------------------------------------------------------------------------
BeginManeuver& BeginManeuver::operator=(const BeginManeuver& begman)
{
   if (&begman == this)
      return *this;
      
   GmatCommand::operator=(begman);
   return *this;
}


std::string BeginManeuver::GetRefObjectName(const Gmat::ObjectType type) const
{
   return GmatCommand::GetRefObjectName(type);
}


const StringArray& BeginManeuver::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return GmatCommand::GetRefObjectNameArray(type);
}


bool BeginManeuver::SetRefObjectName(const Gmat::ObjectType type, 
                                     const std::string &name)
{
   switch (type) {
      case Gmat::SPACECRAFT:
         
         return true;
      
      case Gmat::BURN:
      
         return true;
         
      default:
         ;
   }
   
   return GmatCommand::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginManeuver command.
 *
 * @return clone of the Propagate.
 */
//------------------------------------------------------------------------------
GmatBase* BeginManeuver::Clone() const
{
   return new BeginManeuver(*this);
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/** 
 * The method that is fired to turn on thrusters.
 *
 * @return true if the GmatCommand runs to completion, false if an error 
 *         occurs. 
 */
//------------------------------------------------------------------------------
bool BeginManeuver::Execute()
{
   return false;
}
