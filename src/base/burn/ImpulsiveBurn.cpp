//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the ImpulsiveBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#include "ImpulsiveBurn.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_IMPULSIVE_BURN


//------------------------------------------------------------------------------
//  Burn(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the impulsive burn (default constructor).
 * 
 * @param <nomme> Name for the object
 */
//------------------------------------------------------------------------------
ImpulsiveBurn::ImpulsiveBurn(std::string nomme) :
    Burn            ("ImpulsiveBurn", nomme)
{
}


//------------------------------------------------------------------------------
//  ImpulsiveBurn(const ImpulsiveBurn &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the impulsive burn from the input burn (copy constructor).
 *
 * @param <copy> ImpulsiveBurn object to copy
 */
//------------------------------------------------------------------------------
ImpulsiveBurn::ImpulsiveBurn(const ImpulsiveBurn &copy) :
Burn            (copy)
{
}

//------------------------------------------------------------------------------
//  Burn(void)
//------------------------------------------------------------------------------
/**
 * Destroys the impulsive burn (destructor).
 */
//------------------------------------------------------------------------------
ImpulsiveBurn::~ImpulsiveBurn()
{
}


//------------------------------------------------------------------------------
//  bool Fire(Real *burnData)
//------------------------------------------------------------------------------
/**
 * Applies the burn.  
 * 
 * Provides the mathematics that model an impulsive burn.  The parameter 
 * (burnData) can be used to pass in a Cartesian state (x, y, z, Vx, Vy, Vz) 
 * that needs to incorporate the burn.  If the class has an assigned Spacecraft,
 * that spacecraft is used instead of the input state.
 * 
 * @param <burnData>    Array of data specific to the derived burn class. 
 */
bool ImpulsiveBurn::Fire(Real *burnData)
{
   #ifdef DEBUG_IMPULSIVE_BURN
      MessageInterface::ShowMessage("ImpulsiveBurn::Fire entered for %s\n",
         instanceName.c_str());
   #endif

   frame = frameman->GetFrameInstance(coordFrame);
   if (frame == NULL)
      throw BurnException("Maneuver frame undefined");
    
   PropState *state;
   if (sc)    
      state = &sc->GetState();
   else
      throw BurnException("Maneuver initial state undefined (No spacecraft?)");
    
   // Set the state 6-vector from the associated spacecraft
   frame->SetState(state->GetState());
   // Calculate the maneuver basis vectors
   frame->CalculateBasis(frameBasis);
    
   #ifdef DEBUG_IMPULSIVE_BURN
      MessageInterface::ShowMessage(
         "   Maneuvering spacecraft %s\n",
         sc->GetName().c_str());
      MessageInterface::ShowMessage(
         "   Position for burn:    %18le  %18le  %18le\n",
         (*state)[0], (*state)[1], (*state)[2]);
      MessageInterface::ShowMessage(
         "   Velocity before burn: %18le  %18le  %18le\n",
         (*state)[3], (*state)[4], (*state)[5]);
   #endif

   // Add in the delta-V
   (*state)[3] += deltaV[0]*frameBasis[0][0] +
               deltaV[1]*frameBasis[0][1] +
               deltaV[2]*frameBasis[0][2];
   (*state)[4] += deltaV[0]*frameBasis[1][0] +
               deltaV[1]*frameBasis[1][1] +
               deltaV[2]*frameBasis[1][2];
   (*state)[5] += deltaV[0]*frameBasis[2][0] +
               deltaV[1]*frameBasis[2][1] +
               deltaV[2]*frameBasis[2][2];

   #ifdef DEBUG_IMPULSIVE_BURN
      MessageInterface::ShowMessage(
         "   Velocity after burn:  %18le  %18le  %18le\n",
         (*state)[3], (*state)[4], (*state)[5]);
   #endif

   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ImpulsiveBurn.
 *
 * @return clone of the ImpulsiveBurn.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ImpulsiveBurn::Clone(void) const
{
   return (new ImpulsiveBurn(*this));
}

