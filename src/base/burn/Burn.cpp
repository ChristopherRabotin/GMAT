//$Header$
//------------------------------------------------------------------------------
//                                   Burn
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
 * Defines the Burn base class used for impulsive and finite maneuvers. 
 */
//------------------------------------------------------------------------------


#include "Burn.hpp"


Burn::Burn(std::string typeStr, std::string nomme) :
    GmatBase        (Gmat::BURN, typeStr, nomme),
    coordFrame      ("VNB"),
    coordSystem     ("Cartesian"),
    // Parameter IDs, for convenience
    coordFrameID    (parameterCount),
    coordSystemID   (parameterCount+1),
    deltaV1ID       (parameterCount+2),
    deltaV2ID       (parameterCount+3),
    deltaV3ID       (parameterCount+4),
    deltaV1LabelID  (parameterCount+5),
    deltaV2LabelID  (parameterCount+6),
    deltaV3LabelID  (parameterCount+7)
{
    deltaV[0] = deltaV[1] = deltaV[2] = 0.0;
    dvLabels[0] = "V";
    dvLabels[1] = "N";
    dvLabels[2] = "B";
    
    parameterCount += 8;
}


Burn::~Burn()
{
}


Burn::Burn(const Burn &b) :
    GmatBase        (b),
    coordFrame      (b.coordFrame),
    coordSystem     (b.coordSystem),
    // Parameter IDs, for convenience
    coordFrameID    (b.coordFrameID),
    coordSystemID   (b.coordSystemID),
    deltaV1ID       (b.deltaV1ID),
    deltaV2ID       (b.deltaV2ID),
    deltaV3ID       (b.deltaV3ID),
    deltaV1LabelID  (b.deltaV1LabelID),
    deltaV2LabelID  (b.deltaV2LabelID),
    deltaV3LabelID  (b.deltaV3LabelID)
{
    deltaV[0] = b.deltaV[0];
    deltaV[1] = b.deltaV[1];
    deltaV[2] = b.deltaV[2];
    dvLabels[0] = b.dvLabels[0];
    dvLabels[1] = b.dvLabels[1];
    dvLabels[2] = b.dvLabels[2];
    
    parameterCount = b.parameterCount;
}


Burn Burn::operator=(const Burn &b)
{}


std::string Burn::GetParameterText(const Integer id) const
{
    if (id == coordFrameID) 
        return "CoordinateFrame";
        
    if (id == coordSystemID) 
        return "VectorFormat";
        
    if (id == deltaV1ID) 
        return "Element1";
        
    if (id == deltaV2ID) 
        return "Element2";
        
    if (id == deltaV3ID) 
        return "Element3";
        
    if (id == deltaV1LabelID) 
        return "Element1Label";
        
    if (id == deltaV2LabelID) 
        return "Element2Label";
        
    if (id == deltaV3LabelID) 
        return "Element3Label";
        
    return GmatBase::GetParameterText(id);
}


Integer Burn::GetParameterID(const std::string &str) const
{
    if (str == "CoordinateFrame") 
        return coordFrameID;
        
    if (str == "VectorFormat") 
        return coordSystemID;
        
    if (str == "Element1") 
        return deltaV1ID;
        
    if (str == "Element2") 
        return deltaV2ID;
        
    if (str == "Element3") 
        return deltaV3ID;
        
    if (str == "Element1Label") 
        return deltaV1LabelID;
        
    if (str == "Element2Label") 
        return deltaV2LabelID;
        
    if (str == "Element3Label") 
        return deltaV3LabelID;
        
    return GmatBase::GetParameterID(str);
}


Gmat::ParameterType Burn::GetParameterType(const Integer id) const
{
    if (id == coordFrameID) 
        return Gmat::STRING_TYPE;
        
    if (id == coordSystemID) 
        return Gmat::STRING_TYPE;
        
    if (id == deltaV1ID) 
        return Gmat::REAL_TYPE;
        
    if (id == deltaV2ID) 
        return Gmat::REAL_TYPE;
        
    if (id == deltaV3ID) 
        return Gmat::REAL_TYPE;
        
    if (id == deltaV1LabelID) 
        return Gmat::STRING_TYPE;
        
    if (id == deltaV2LabelID) 
        return Gmat::STRING_TYPE;
        
    if (id == deltaV3LabelID) 
        return Gmat::STRING_TYPE;
        
    return GmatBase::GetParameterType(id);
}


std::string Burn::GetParameterTypeString(const Integer id) const
{
    if (id == coordFrameID) 
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == coordSystemID) 
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == deltaV1ID) 
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
        
    if (id == deltaV2ID) 
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
        
    if (id == deltaV3ID) 
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
        
    if (id == deltaV1LabelID) 
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == deltaV2LabelID) 
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == deltaV3LabelID) 
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    return GmatBase::GetParameterTypeString(id);
}


Real Burn::GetRealParameter(const Integer id) const
{
    if (id == deltaV1ID) 
        return deltaV[0];
        
    if (id == deltaV2ID) 
        return deltaV[1];
        
    if (id == deltaV3ID) 
        return deltaV[2];
        
    return GmatBase::GetRealParameter(id);
}


Real Burn::SetRealParameter(const Integer id, const Real value)
{
    if (id == deltaV1ID) {
        deltaV[0] = value;
        return deltaV[0];
    }
        
    if (id == deltaV2ID) {
        deltaV[1] = value;
        return deltaV[1];
    }
        
    if (id == deltaV3ID) {
        deltaV[2] = value;
        return deltaV[2];
    } 

    return GmatBase::SetRealParameter(id, value);
}


std::string Burn::GetStringParameter(const Integer id) const
{
    if (id == coordFrameID) 
        return coordFrame;
        
    if (id == coordSystemID) 
        return coordSystem;
        
    if (id == deltaV1LabelID) 
        return dvLabels[0];
        
    if (id == deltaV2LabelID) 
        return dvLabels[1];
        
    if (id == deltaV3LabelID) 
        return dvLabels[2];
        
    return GmatBase::GetStringParameter(id);
}


bool Burn::SetStringParameter(const Integer id, const std::string &value)
{
    if (id == coordFrameID) {
        /// @todo validate the input value
        // if (!IsValidFrame(value))
        //    return false;    
        coordFrame = value;
        return true;
    }
        
    if (id == coordSystemID) {
        /// @todo validate the input value
        // if (!IsValidSystem(value))
        //     return false;
        coordSystem = value;
        return true;
    }
        
    // Cannot change the labels -- they are set internally
    if (id == deltaV1LabelID) 
        return false;
        
    if (id == deltaV2LabelID) 
        return false;
        
    if (id == deltaV3LabelID) 
        return false;
        
    return GmatBase::SetStringParameter(id, value);
}
