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


//------------------------------------------------------------------------------
//  Burn(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the Burn object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
Burn::Burn(std::string typeStr, std::string nomme) :
    GmatBase        (Gmat::BURN, typeStr, nomme),
    coordFrame      ("Inertial"),
    coordSystem     ("Cartesian"),
    satName         (""),
    sc              (NULL),
    // Parameter IDs, for convenience
    coordFrameID    (parameterCount),
    coordSystemID   (parameterCount+1),
    deltaV1ID       (parameterCount+2),
    deltaV2ID       (parameterCount+3),
    deltaV3ID       (parameterCount+4),
    deltaV1LabelID  (parameterCount+5),
    deltaV2LabelID  (parameterCount+6),
    deltaV3LabelID  (parameterCount+7),
    satNameID       (parameterCount+8)
{
    parameterCount += 9;
    
    deltaV[0] = deltaV[1] = deltaV[2] = 0.0;
    frameman = new ManeuverFrameManager;

    /// Load the default maneuver frame
    frame = frameman->GetFrameInstance(coordFrame);

    dvLabels[0] = frame->GetFrameLabel(1);
    dvLabels[1] = frame->GetFrameLabel(2);
    dvLabels[2] = frame->GetFrameLabel(3);
    
    frameBasis[0][0] = frameBasis[1][1] = frameBasis[2][2] = 1.0;
    frameBasis[0][1] = frameBasis[1][0] = frameBasis[2][0] = 
    frameBasis[0][2] = frameBasis[1][2] = frameBasis[2][1] = 0.0;
     
}


//------------------------------------------------------------------------------
//  ~Burn(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Burn object (destructor).
 */
//------------------------------------------------------------------------------
Burn::~Burn()
{
}


//------------------------------------------------------------------------------
//  Burn(const Burn &b)
//------------------------------------------------------------------------------
/**
 * Constructs the Burn object (copy constructor).
 * 
 * @param <b> Object that is copied
 */
//------------------------------------------------------------------------------
Burn::Burn(const Burn &b) :
    GmatBase        (b),
    coordFrame      (b.coordFrame),
    coordSystem     (b.coordSystem),
    satName         (b.satName),
    // Parameter IDs, for convenience
    coordFrameID    (b.coordFrameID),
    coordSystemID   (b.coordSystemID),
    deltaV1ID       (b.deltaV1ID),
    deltaV2ID       (b.deltaV2ID),
    deltaV3ID       (b.deltaV3ID),
    deltaV1LabelID  (b.deltaV1LabelID),
    deltaV2LabelID  (b.deltaV2LabelID),
    deltaV3LabelID  (b.deltaV3LabelID),
    satNameID       (b.satNameID)
{
    deltaV[0] = b.deltaV[0];
    deltaV[1] = b.deltaV[1];
    deltaV[2] = b.deltaV[2];
    dvLabels[0] = b.dvLabels[0];
    dvLabels[1] = b.dvLabels[1];
    dvLabels[2] = b.dvLabels[2];
    
    parameterCount = b.parameterCount;
}


//------------------------------------------------------------------------------
//  Burn& operator=(const Burn &b)
//------------------------------------------------------------------------------
/**
 * Sets one burn object to match another (assignment operator).
 * 
 * @param <b> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Burn& Burn::operator=(const Burn &b)
{
    if (this == &b)
        return *this;
        
    return *this;
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param <id> Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
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
        
    if (id == satNameID) 
        return "SpacecraftName";
        
    return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param <str> Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
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
        
    if (str == "SpacecraftName") 
        return satNameID;
        
    return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
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
        
    if (id == satNameID) 
        return Gmat::STRING_TYPE;
        
    return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
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
        
    if (id == satNameID) 
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    return GmatBase::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a Real parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Real parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
//  Real GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
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
        
    if (id == satNameID) 
        return satName;
        
    return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool Burn::SetStringParameter(const Integer id, const std::string &value)
{
    if (id == coordFrameID) {
        /// @todo validate the input value
        // if (!IsValidFrame(value))
        //    return false;    
        coordFrame = value;
        frame = frameman->GetFrameInstance(coordFrame);

        dvLabels[0] = frame->GetFrameLabel(1);
        dvLabels[1] = frame->GetFrameLabel(2);
        dvLabels[2] = frame->GetFrameLabel(3);
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
        
    if (id == satNameID) {
        satName = value;
        return true;
    }
        
    return GmatBase::SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 * 
 * For the Burn classes, calls to this method get passed to the maneuver frame
 * manager when the user requests the frames that are available for the system.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
StringArray& Burn::GetStringArrayParameter(const Integer id) const
{
    if (id == coordFrameID)
        return frameman->GetSupportedFrames();

    return GmatBase::GetStringArrayParameter(id);
}
