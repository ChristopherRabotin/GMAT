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


//---------------------------------
// static data
//---------------------------------

/// Labels used for the thruster element parameters.
const std::string
Burn::PARAMETER_TEXT[BurnParamCount - GmatBaseParamCount] =
{
   "CoordinateFrame",
   "VectorFormat",
   "Element1",
   "Element2",
   "Element3",
   "Element1Label",
   "Element2Label",
   "Element3Label",
   "SpacecraftName"
};

/// Types of the parameters used by thrusters.
const Gmat::ParameterType
Burn::PARAMETER_TYPE[BurnParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE, 
   Gmat::REAL_TYPE, 
   Gmat::REAL_TYPE, 
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Burn(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the Burn object (default constructor).
 * 
 * @param typeStr String text identifying the object type
 * @param nomme   Name for the object
 */
//------------------------------------------------------------------------------
Burn::Burn(const std::string &typeStr, const std::string &nomme) :
    GmatBase        (Gmat::BURN, typeStr, nomme),
    coordFrame      ("Inertial"),
    coordSystem     ("Cartesian"),
    satName         (""),
    sc              (NULL)
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
   delete frameman;
}


//------------------------------------------------------------------------------
//  Burn(const Burn &b)
//------------------------------------------------------------------------------
/**
 * Constructs the Burn object (copy constructor).
 * 
 * @param b Object that is copied
 */
//------------------------------------------------------------------------------
Burn::Burn(const Burn &b) :
    GmatBase        (b),
    coordFrame      (b.coordFrame),
    coordSystem     (b.coordSystem),
    satName         (b.satName),
    sc              (NULL)
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
 * @param b The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Burn& Burn::operator=(const Burn &b)
{
    if (this == &b)
        return *this;
        
    GmatBase::operator=(b);

    coordFrame  = b.coordFrame;
    coordSystem = b.coordSystem;
    satName     = b.satName;
    sc          = NULL;
    deltaV[0]   = b.deltaV[0];
    deltaV[1]   = b.deltaV[1];
    deltaV[2]   = b.deltaV[2];
    dvLabels[0] = b.dvLabels[0];
    dvLabels[1] = b.dvLabels[1];
    dvLabels[2] = b.dvLabels[2];

    return *this;
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param id Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string Burn::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < BurnParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param str Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer Burn::GetParameterID(const std::string &str) const
{
    // Let users ask for components (e.g. "V", "N", or "B" in VNB coordinates) 
    // directly
    if (str == dvLabels[0])
        return DELTAV1;
        
    if (str == dvLabels[1]) 
        return DELTAV2;
        
    if (str == dvLabels[2])
        return DELTAV3;
   
   for (Integer i = GmatBaseParamCount; i < BurnParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 * 
 * @param id Integer ID of the parameter.
 * 
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Burn::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < BurnParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
      
   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param id Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Burn::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a Real parameter.
 * 
 * @param id Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
Real Burn::GetRealParameter(const Integer id) const
{
    if (id == DELTAV1) 
        return deltaV[0];
        
    if (id == DELTAV2) 
        return deltaV[1];
        
    if (id == DELTAV3) 
        return deltaV[2];
        
    return GmatBase::GetRealParameter(id);
}

      
//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Real parameter.
 * 
 * @param id Integer ID of the parameter.
 * @param value New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
Real Burn::SetRealParameter(const Integer id, const Real value)
{
    if (id == DELTAV1) {
        deltaV[0] = value;
        return deltaV[0];
    }
        
    if (id == DELTAV2) {
        deltaV[1] = value;
        return deltaV[1];
    }
        
    if (id == DELTAV3) {
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
 * @param id Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string Burn::GetStringParameter(const Integer id) const
{
    if (id == COORDFRAME) 
        return coordFrame;
        
    if (id == COORDSYSTEM) 
        return coordSystem;
        
    if (id == DELTAV1LABEL) 
        return dvLabels[0];
        
    if (id == DELTAV2LABEL) 
        return dvLabels[1];
        
    if (id == DELTAV3LABEL) 
        return dvLabels[2];
        
    if (id == SATNAME) 
        return satName;
        
    return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param id Integer ID of the parameter.
 * @param value New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool Burn::SetStringParameter(const Integer id, const std::string &value)
{
    if (id == COORDFRAME) {
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
        
    if (id == COORDSYSTEM) {
        /// @todo validate the input value
        // if (!IsValidSystem(value))
        //     return false;
        coordSystem = value;
        return true;
    }
        
    // Cannot change the labels -- they are set internally
    if (id == DELTAV1LABEL) 
        return false;
        
    if (id == DELTAV2LABEL) 
        return false;
        
    if (id == DELTAV3LABEL) 
        return false;
        
    if (id == SATNAME) {
        satName = value;
        return true;
    }
        
    return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  Real SetStringParameter(const Integer id, const Real value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a specific std::string element in an array.
 *
 * @param id Integer ID of the parameter.
 * @param value New value for the parameter.
 * @param index Index for the element
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool Burn::SetStringParameter(const Integer id, const std::string &value,
                              const Integer index)
{
   return GmatBase::SetStringParameter(id, value, index);
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
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& Burn::GetStringArrayParameter(const Integer id) const
{
    if (id == COORDFRAME)
        return frameman->GetSupportedFrames();

    return GmatBase::GetStringArrayParameter(id);
}


// 
//---------------------------------------------------------------------------
//  void SetSpacecraftToManeuver(Spacecraft *sat)
//---------------------------------------------------------------------------
/**
 * Accessor method used by Maneuver to pass in the spacecraft pointer
 * 
 * @param sat the Spacecraft
 */
//---------------------------------------------------------------------------
void Burn::SetSpacecraftToManeuver(Spacecraft *sat)
{
    sc = sat;
}
