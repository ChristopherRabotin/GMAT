//$Header$
//------------------------------------------------------------------------------
//                                   SMA
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
 * Implements a functional semimajor axis parameter class. 
 */
//------------------------------------------------------------------------------


#include "SMA.hpp"
#include "ParameterException.hpp"


SMA::SMA(const std::string &name) :
    Parameter   (name, "SMA", SYSTEM_PARAM, NULL, "", "km", false),
    results     (NULL)
{
    cbLoc = new Real[3];    // Eventually set from the solar system
    mu    = 398600.4415;    // Eventually set from the solar system
    
    cbLoc[0] = cbLoc[1] = cbLoc[2] = 0.0;
}


SMA::~SMA()
{
    if (results)
        delete [] results;
        
    delete [] cbLoc;
}


SMA::SMA(const SMA &sma) :
    Parameter   (sma),
    results     (NULL)
{
    cbLoc = new Real[3];    // Eventually set from the solar system
    mu    = 398600.4415;    // Eventually set from the solar system
    
    cbLoc[0] = cbLoc[1] = cbLoc[2] = 0.0;
}


SMA& SMA::operator=(const SMA &sma)
{
    if (this == &sma)
        return *this;
 
    if (results) {
        delete [] results;
        results = NULL;
    }
    
    Parameter::operator=(sma);
    return *this;
}


bool SMA::AddObject(GmatBase *object)
{
    if (!object)
        return false;
        
    if (object->GetType() != Gmat::SPACECRAFT)
        throw ParameterException("SMA Parameter class only supports spacecraft");

    source.push_back((Spacecraft*)(object));
    return true;
}


Integer SMA::GetNumObjects() const
{
    return source.size();
}


bool SMA::Evaluate()
{
    if (source.size() == 0)
        return false;
        
    if (results == NULL)
        results = new Real[source.size()];
        
    Integer i = 0, id;
    std::vector<Spacecraft *>::iterator current;
    Real energy, radius, speed2, dist[3];
    
    for (current = source.begin(); current != source.end(); ++current) {
        id = (*current)->GetParameterID("CoordinateRepresentation");
        if ((*current)->GetStringParameter(id) != "Cartesian")
            throw ParameterException("SMA needs a Cartesian state");
        PropState state = (*current)->GetState();
        dist[0] = state[0] - cbLoc[0];
        dist[1] = state[1] - cbLoc[1];
        dist[2] = state[2] - cbLoc[2];
        radius = sqrt(dist[0]*dist[0] + dist[1]*dist[1] + dist[2]*dist[2]);
        speed2 = state[3]*state[3] + state[4]*state[4] + state[5]*state[5];
        energy = speed2 / 2.0 - mu / radius;
        results[i] = - mu / (2.0 * energy);
        ++i;
    }
    
    return true;
}


Real SMA::EvaluateReal()
{
    if (!Evaluate())
        throw ParameterException("SMA failed evaluation");
    return results[0];      // return the first one for now
}


bool SMA::Validate()
{
    return true;
}

