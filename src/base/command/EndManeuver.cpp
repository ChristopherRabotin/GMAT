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
 *  Implementation code for the EndManeuver command.
 */
//------------------------------------------------------------------------------

#include "EndManeuver.hpp"


//------------------------------------------------------------------------------
// EndManeuver()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
EndManeuver::EndManeuver() :
   GmatCommand    ("EndManeuver")
{
   if (instanceName == "")
      instanceName = "EndManeuver";
}


//------------------------------------------------------------------------------
// ~EndManeuver()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
EndManeuver::~EndManeuver()
{
}


//------------------------------------------------------------------------------
// EndManeuver(const BeginManeuver& endman)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param endman The command that gets copied.
 */
//------------------------------------------------------------------------------
EndManeuver::EndManeuver(const EndManeuver& endman) :
   GmatCommand       (endman)
{
}


//------------------------------------------------------------------------------
// EndManeuver& operator=(const EndManeuver& endman)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param endman The command that gets copied.
 * 
 * @return this instance, with internal data structures set to match the input
 *         instance.
 */
//------------------------------------------------------------------------------
EndManeuver& EndManeuver::operator=(const EndManeuver& endman)
{
   if (&endman == this)
      return *this;
      
   GmatCommand::operator=(endman);
   return *this;
}


std::string EndManeuver::GetRefObjectName(const Gmat::ObjectType type) const
{
   return GmatCommand::GetRefObjectName(type);
}


const StringArray& EndManeuver::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return GmatCommand::GetRefObjectNameArray(type);
}


bool EndManeuver::SetRefObjectName(const Gmat::ObjectType type, 
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
 * This method returns a clone of the EndManeuver command.
 *
 * @return clone of the Propagate.
 */
//------------------------------------------------------------------------------
GmatBase* EndManeuver::Clone() const
{
   return new EndManeuver(*this);
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/** 
 * The method that is fired to turn off thrusters.
 *
 * @return true if the GmatCommand runs to completion, false if an error 
 *         occurs. 
 */
//------------------------------------------------------------------------------
bool EndManeuver::Execute()
{
   return false;
}
