//$Header$
//------------------------------------------------------------------------------
//                                  GmatBase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2003/10/22
//
/**
 * Implements the Spacecraft base class. 
 *
 */
//------------------------------------------------------------------------------

#include "Spacecraft.hpp"

/// Set the default values for spacecraft 
const Real        Spacecraft::EPOCH = 21545.0; 
const Real        Spacecraft::SEMI_MAJOR_AXIS = 7000.0; 
const Real        Spacecraft::INCLINATION = 1000.0; 
const Real        Spacecraft::ARG_OF_PERIGEE = 7.9; 
const std::string Spacecraft::REF_BODY= "Earth"; 
const std::string Spacecraft::REF_FRAME= "Cartesian"; 
const std::string Spacecraft::REF_PLANE= "Equatorial"; 

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  Spacecraft()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
Spacecraft::Spacecraft() : 
    GmatBase     (Gmat::SPACECRAFT,"Spacecraft",""),
    epochID      (parameterCount),
    state1ID     (parameterCount + 1),
    state2ID     (parameterCount + 2),
    state3ID     (parameterCount + 3),
    state4ID     (parameterCount + 4),
    state5ID     (parameterCount + 5),
    state6ID     (parameterCount + 6),
    refBodyID    (parameterCount + 7),
    refFrameID   (parameterCount + 8),
    refPlaneID   (parameterCount + 9)
{
    parameterCount += 10;
}

//---------------------------------------------------------------------------
//  Spacecraft(Gmat::ObjectTypes typeId, std::string &typeStr, 
//             std::string &nomme)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters. 
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 * @param <nomme> Optional name for the object.  Defaults to "".
 *
 */
Spacecraft::Spacecraft(const std::string &typeStr, const std::string &noname) :
    GmatBase(Gmat::SPACECRAFT,typeStr,noname),
    epochID(parameterCount),
    state1ID(parameterCount + 1),
    state2ID(parameterCount + 2),
    state3ID(parameterCount + 3),
    state4ID(parameterCount + 4),
    state5ID(parameterCount + 5),
    state6ID(parameterCount + 6),
    refBodyID(parameterCount + 7),
    refFrameID(parameterCount + 8),
    refPlaneID(parameterCount + 9)
{
    parameterCount += 10;
}

//---------------------------------------------------------------------------
//  Spacecraft(const Spacecraft &a)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base Spacecraft structures.
 *
 * @param <a> The original that is being copied.
 */
Spacecraft::Spacecraft(const Spacecraft &a) :
    GmatBase (a),
    epoch    (a.epoch),
    refBody  (a.refBody),
    refFrame (a.refFrame),
    refPlane (a.refPlane)
{
    for (int i = 0; i < 6; ++i)
       state[i] = a.state[i];
}

//---------------------------------------------------------------------------
//  ~Spacecraft(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
Spacecraft::~Spacecraft(void)
{
}

//---------------------------------------------------------------------------
//  Spacecraft& operator=(const Spacecraft &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for Spacecraft structures.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 */
Spacecraft& Spacecraft::operator=(const Spacecraft &a)
{
    // Don't do anything if copying self
    if (&a == this)
        return *this;
        
    // Currently nothing to do from the base class; this may change in a later 
    // build

    return *this;
}

//---------------------------------------------------------------------------
//  std::string GetName(void) const
//---------------------------------------------------------------------------
/**
 * Retrieve the name of the instance.
 *
 * @return This object's name, or the empty string ("").
 *
 * @note Some classes are unnamed.
 */
std::string Spacecraft::GetName(void) const
{
    return instanceName;
}

//---------------------------------------------------------------------------
//  bool SetName(std::string &name)
//---------------------------------------------------------------------------
/**
 * Set the name for this instance.
 *
 * @param <name> the object's name.
 *
 * @return true if the name was changed, false if an error was encountered.
 *
 */
bool Spacecraft::SetName(const std::string &name)
{
    instanceName = name;
    return true;
}

//---------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//---------------------------------------------------------------------------
/**
 * Retrieve the ID for the parameter given its description.
 *
 * @param <str> Description for the parameter.
 *
 * @return the parameter ID, or -1 if there is no associated ID.
 */
Integer Spacecraft::GetParameterID(const std::string &str) const
{
    return -1;
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
Real Spacecraft::GetRealParameter(const Integer id) const
{
    switch(id)
    {
        epochID  : return epoch;
        stateID1 : return state[0];
        stateID2 : return state[1];
        stateID3 : return state[2];
        stateID4 : return state[3];
        stateID5 : return state[4];
        stateID6 : return state[5];
        default  : return REAL_PARAMETER_UNDEFINED;
    }
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or 
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the 
 *         parameter type is not Real.
 */
Real Spacecraft::SetRealParameter(const Integer id, const Real value)
{
    switch(id)
    {
        epochID  : return epoch = value;
        stateID1 : return state[0] = value;
        stateID2 : return state[1] = value;
        stateID3 : return state[2] = value;
        stateID4 : return state[3] = value;
        stateID5 : return state[4] = value;
        stateID6 : return state[5] = value;
        default  : return REAL_PARAMETER_UNDEFINED;
    }
}

//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
std::string Spacecraft::GetStringParameter(const Integer id) const
{
    if (id == refBodyID)
       return refBody; 

    if (id == refFrameID)
       return refFrame;

    if (id == refPlaneID)
       return refPlane;

    return "";
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
bool Spacecraft::SetStringParameter(const Integer id, const std::string &value)
{
    if (id != refBodyID && id != refFrameID && id != refPlaneID)
       return false;

    if (id == refBodyID)
       refBody = value; 
    else if (id == refFrameID)
       refFrame = value;
    else 
       refPlane = value;

    return true;
}
