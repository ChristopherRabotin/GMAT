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


ImpulsiveBurn::ImpulsiveBurn(std::string nomme) :
    Burn            ("ImpulsiveBurn", nomme)
{}


ImpulsiveBurn::~ImpulsiveBurn()
{}


bool ImpulsiveBurn::Fire(Real *burnData)
{
    frame = frameman->GetFrameInstance(coordFrame);
    if (frame == NULL)
        throw BurnException("Maneuver frame undefined");
    
    Real *state = burnData;
    if (sc)    
        state = sc->GetState();
        
    if (state == NULL)
        throw BurnException("Maneuver initial state undefined");
    
    // Set the state 6-vector from the associated spacecraft
    frame->SetState(state);
    // Calculate the maneuver basis vectors
    frame->CalculateBasis(frameBasis);
    
    // Add in the delta-V
    state[3] += deltaV[0]*frameBasis[0][0] +
                deltaV[1]*frameBasis[0][1] +
                deltaV[2]*frameBasis[0][2];
    state[4] += deltaV[0]*frameBasis[1][0] +
                deltaV[1]*frameBasis[1][1] +
                deltaV[2]*frameBasis[1][2];
    state[5] += deltaV[0]*frameBasis[2][0] +
                deltaV[1]*frameBasis[2][1] +
                deltaV[2]*frameBasis[2][2];
}
