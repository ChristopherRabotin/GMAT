//$Header$
//------------------------------------------------------------------------------
//                                   Inc
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Implements a functional inclination parameter class. 
 */
//------------------------------------------------------------------------------


#include "Inc.hpp"
#include "ParameterException.hpp"


Inc::Inc(const std::string &name) :
    Parameter   (name, "Inc", SYSTEM_PARAM, NULL, "", "deg", false),
    results     (NULL)
{
    cbLoc = new Real[3];    // Eventually set from the solar system
    cbLoc[0] = cbLoc[1] = cbLoc[2] = 0.0;
}


Inc::~Inc()
{
    if (results)
        delete [] results;

    delete [] cbLoc;
}


Inc::Inc(const Inc &Inc) :
    Parameter   (Inc),
    results     (NULL)
{
    cbLoc = new Real[3];    // Eventually set from the solar system
    cbLoc[0] = cbLoc[1] = cbLoc[2] = 0.0;
}


Inc& Inc::operator=(const Inc &Inc)
{
    if (this == &Inc)
        return *this;
 
    if (results) {
        delete [] results;
        results = NULL;
    }
    
    Parameter::operator=(Inc);
    return *this;
}


bool Inc::AddObject(GmatBase *object)
{
    if (!object)
        return false;
        
    if (object->GetType() != Gmat::SPACECRAFT)
        throw ParameterException("Inc Parameter class only supports spacecraft");

    source.push_back((Spacecraft*)(object));
}


Integer Inc::GetNumObjects() const
{
    return source.size();
}


bool Inc::Evaluate()
{
    if (source.size() == 0)
        return false;
        
    if (results == NULL)
        results = new Real[source.size()];

    Integer i = 0, id;
    std::vector<Spacecraft *>::iterator current;
    Real mag, rCrossV[3], *state, dist[3];
    
    for (current = source.begin(); current != source.end(); ++current) {
        id = (*current)->GetParameterID("CoordinateRepresentation");
        if ((*current)->GetStringParameter(id) != "Cartesian")
            throw ParameterException("Inc needs a Cartesian state");
        state = (*current)->GetState();
        dist[0] = state[0] - cbLoc[0];
        dist[1] = state[1] - cbLoc[1];
        dist[2] = state[2] - cbLoc[2];
        
        rCrossV[0] = dist[1] * state[5] - dist[2] * state[4];
        rCrossV[1] = dist[2] * state[3] - dist[0] * state[5];
        rCrossV[2] = dist[0] * state[4] - dist[1] * state[3];

        mag = sqrt(rCrossV[0] * rCrossV[0] +
                   rCrossV[1] * rCrossV[1] +
                   rCrossV[2] * rCrossV[2]);

        if (mag == 0.0)
            throw("R x V == 0.0 in Inc parameter");
        mag = rCrossV[2] / mag;

        if (fabs(mag) > 1.0 + 1.0e-12)
            throw("Bad arccos in Inc parameter");
        if (fabs(mag) > 1.0)
            mag = ((mag > 0.0) ? 1.0 : -1.0);
        
        
        results[i] = 180.0 / M_PI * acos(mag);
        ++i;
    }
    
    return true;
}


Real Inc::EvaluateReal()
{
    if (!Evaluate())
        throw ParameterException("Inc failed evaluation");
    return results[0];      // return the first one for now
}


bool Inc::Validate()
{
    return true;
}

