//$Header$
//------------------------------------------------------------------------------
//                                   Ecc
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
 * Implements a functional eccentricity parameter class. 
 */
//------------------------------------------------------------------------------


#include "Ecc.hpp"
#include "ParameterException.hpp"


Ecc::Ecc(const std::string &name) :
    Parameter   (name, "Ecc", SYSTEM_PARAM, NULL, "", "", false),
    results     (NULL)
{
    cbLoc = new Real[3];    // Eventually set from the solar system
    mu    = 398600.4415;    // Eventually set from the solar system
    
    cbLoc[0] = cbLoc[1] = cbLoc[2] = 0.0;
}


Ecc::~Ecc()
{
    if (results)
        delete [] results;
}


Ecc::Ecc(const Ecc &Ecc) :
    Parameter   (Ecc),
    results     (NULL)
{
    cbLoc = new Real[3];    // Eventually set from the solar system
    mu    = 398600.4415;    // Eventually set from the solar system
    
    cbLoc[0] = cbLoc[1] = cbLoc[2] = 0.0;
}


Ecc& Ecc::operator=(const Ecc &Ecc)
{
    if (this == &Ecc)
        return *this;
 
    if (results) {
        delete [] results;
        results = NULL;
    }
    
    Parameter::operator=(Ecc);
    return *this;
}


bool Ecc::AddObject(GmatBase *object)
{
    if (!object)
        return false;
        
    if (object->GetType() != Gmat::SPACECRAFT)
        throw ParameterException("Ecc Parameter class only supports spacecraft");

    source.push_back((Spacecraft*)(object));
    return true;
}



Integer Ecc::GetNumObjects() const
{
    return source.size();
}


bool Ecc::Evaluate()
{
    if (source.size() == 0)
        return false;
        
    if (results == NULL)
        results = new Real[source.size()];

    Integer i = 0, id;
    std::vector<Spacecraft *>::iterator current;
    Real mag, rCrossV[3], vCrossRCrossV[3], dist[3], eVect[3];
    //PropState *state;
    
    for (current = source.begin(); current != source.end(); ++current) {
        id = (*current)->GetParameterID("CoordinateRepresentation");
        if ((*current)->GetStringParameter(id) != "Cartesian")
            throw ParameterException("Inc needs a Cartesian state");
        PropState state = (*current)->GetState();
        dist[0] = state[0] - cbLoc[0];
        dist[1] = state[1] - cbLoc[1];
        dist[2] = state[2] - cbLoc[2];
        
        rCrossV[0] = dist[1] * state[5] - dist[2] * state[4];
        rCrossV[1] = dist[2] * state[3] - dist[0] * state[5];
        rCrossV[2] = dist[0] * state[4] - dist[1] * state[3];

        vCrossRCrossV[0] = state[4] * rCrossV[2] - state[5] * rCrossV[1];
        vCrossRCrossV[1] = state[5] * rCrossV[0] - state[3] * rCrossV[2];
        vCrossRCrossV[2] = state[3] * rCrossV[1] - state[4] * rCrossV[0];

        mag = sqrt(dist[0] * dist[0] + dist[1] * dist[1] + dist[2] * dist[2]);

        if (mag == 0.0)
            throw("R == 0.0 in Ecc parameter");
            
        eVect[0] = vCrossRCrossV[0] / mu - dist[0] / mag;
        eVect[1] = vCrossRCrossV[1] / mu - dist[1] / mag;
        eVect[2] = vCrossRCrossV[2] / mu - dist[2] / mag;

        results[i] = sqrt(eVect[0]*eVect[0] + 
                          eVect[1]*eVect[1] + 
                          eVect[2]*eVect[2]);
        ++i;
    }
    
    return true;
}


Real Ecc::EvaluateReal()
{
    if (!Evaluate())
        throw ParameterException("Ecc failed evaluation");
    return results[0];      // return the first one for now
}


bool Ecc::Validate()
{
    return true;
}

