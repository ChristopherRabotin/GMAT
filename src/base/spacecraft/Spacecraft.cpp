//$Header$
//------------------------------------------------------------------------------
//                                  Spacecraft 
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
#include <iostream>     // will remove it later
#include <sstream>     // will remove it later

/// Set the default values for spacecraft 
const Real        Spacecraft::EPOCH      = 21545.0; 
const Real        Spacecraft::ELEMENT1   = 7100.0;
const Real        Spacecraft::ELEMENT2   = 0.0;
const Real        Spacecraft::ELEMENT3   = 1300.0; 
const Real        Spacecraft::ELEMENT4   = 0.0;
const Real        Spacecraft::ELEMENT5   = 7.35; 
const Real        Spacecraft::ELEMENT6   = 1.0;
const std::string Spacecraft::DATEFORMAT = "TAIModJulian";
const std::string Spacecraft::STATE_TYPE = "Cartesian"; 
const std::string Spacecraft::REF_BODY   = "Earth"; 
const std::string Spacecraft::REF_FRAME  = "MJ2000"; 
const std::string Spacecraft::REF_PLANE  = "Equatorial"; 

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
    GmatBase       (Gmat::SPACECRAFT,"Spacecraft",""),
    epochID        (parameterCount + EPOCH_ID),
    state1ID       (parameterCount + ELEMENT1_ID),
    state2ID       (parameterCount + ELEMENT2_ID),
    state3ID       (parameterCount + ELEMENT3_ID),
    state4ID       (parameterCount + ELEMENT4_ID),
    state5ID       (parameterCount + ELEMENT5_ID),
    state6ID       (parameterCount + ELEMENT6_ID),
    stateTypeID    (parameterCount + STATE_TYPE_ID),
    refBodyID      (parameterCount + BODY_ID),
    refFrameID     (parameterCount + FRAME_ID),
    refPlaneID     (parameterCount + PLANE_ID),
    dryMassID      (parameterCount + DRY_MASS_ID),
    dateFormatID   (parameterCount + DATE_FORMAT_ID),
    coeffDragID    (parameterCount + COEFF_DRAG_ID),
    dragAreaID     (parameterCount + DRAG_AREA_ID),
    srpAreaID      (parameterCount + SRP_AREA_ID),
    reflectCoeffID (parameterCount + REFLECT_COEFF_ID),
    solarSystem    (NULL)
{
    parameterCount += SC_ParameterIDs;
    InitializeValues();
}

//---------------------------------------------------------------------------
//  Spacecraft(const std::string &name)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <name> Optional name for the object.  Defaults to "".
 *
 */
Spacecraft::Spacecraft(const std::string &name) :
    GmatBase       (Gmat::SPACECRAFT, "Spacecraft", name),
    epochID        (parameterCount + EPOCH_ID),
    state1ID       (parameterCount + ELEMENT1_ID),
    state2ID       (parameterCount + ELEMENT2_ID),
    state3ID       (parameterCount + ELEMENT3_ID),
    state4ID       (parameterCount + ELEMENT4_ID),
    state5ID       (parameterCount + ELEMENT5_ID),
    state6ID       (parameterCount + ELEMENT6_ID),
    stateTypeID    (parameterCount + STATE_TYPE_ID),
    refBodyID      (parameterCount + BODY_ID),
    refFrameID     (parameterCount + FRAME_ID),
    refPlaneID     (parameterCount + PLANE_ID),
    dryMassID      (parameterCount + DRY_MASS_ID),
    dateFormatID   (parameterCount + DATE_FORMAT_ID),
    coeffDragID    (parameterCount + COEFF_DRAG_ID),
    dragAreaID     (parameterCount + DRAG_AREA_ID),
    srpAreaID      (parameterCount + SRP_AREA_ID),
    reflectCoeffID (parameterCount + REFLECT_COEFF_ID),
    solarSystem    (NULL)
{
    parameterCount += SC_ParameterIDs;
    InitializeValues();
}

//---------------------------------------------------------------------------
//  Spacecraft(const std::string &typeStr, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 * @param <name> Optional name for the object.  Defaults to "".
 *
 */
Spacecraft::Spacecraft(const std::string &typeStr, const std::string &name) :
    GmatBase       (Gmat::SPACECRAFT, typeStr, name),
    epochID        (parameterCount + EPOCH_ID),
    state1ID       (parameterCount + ELEMENT1_ID),
    state2ID       (parameterCount + ELEMENT2_ID),
    state3ID       (parameterCount + ELEMENT3_ID),
    state4ID       (parameterCount + ELEMENT4_ID),
    state5ID       (parameterCount + ELEMENT5_ID),
    state6ID       (parameterCount + ELEMENT6_ID),
    stateTypeID    (parameterCount + STATE_TYPE_ID),
    refBodyID      (parameterCount + BODY_ID),
    refFrameID     (parameterCount + FRAME_ID),
    refPlaneID     (parameterCount + PLANE_ID),
    dryMassID      (parameterCount + DRY_MASS_ID),
    dateFormatID   (parameterCount + DATE_FORMAT_ID),
    coeffDragID    (parameterCount + COEFF_DRAG_ID),
    dragAreaID     (parameterCount + DRAG_AREA_ID),
    srpAreaID      (parameterCount + SRP_AREA_ID),
    reflectCoeffID (parameterCount + REFLECT_COEFF_ID),
    solarSystem    (NULL)
{
    parameterCount += SC_ParameterIDs;
    InitializeValues();
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
    GmatBase       (a),
    epoch          (a.epoch),
    dateFormat     (a.dateFormat),
    stateType      (a.stateType),
    refBody        (a.refBody),
    refFrame       (a.refFrame),
    refPlane       (a.refPlane),
    epochID        (a.epochID),
    state1ID       (a.state1ID),
    state2ID       (a.state2ID),
    state3ID       (a.state3ID),
    state4ID       (a.state4ID),
    state5ID       (a.state5ID),
    state6ID       (a.state6ID),
    stateTypeID    (a.stateTypeID),
    refBodyID      (a.refBodyID),
    refFrameID     (a.refFrameID),
    refPlaneID     (a.refPlaneID),
    dryMassID      (a.dryMassID),
    dateFormatID   (a.dateFormatID),
    coeffDragID    (a.coeffDragID),
    dragAreaID     (a.dragAreaID),
    srpAreaID      (a.srpAreaID),
    reflectCoeffID (a.reflectCoeffID),
    solarSystem    (a.solarSystem)
{
    parameterCount = a.parameterCount;
    for (int i = 0; i < 6; ++i)
    { 
       state[i] = a.state[i];
       displayState[i] = a.displayState[i];
    }
    displayEpoch = a.displayEpoch;
    displayDateFormat = a.displayDateFormat;
    displayCoordType = a.displayCoordType;
    initialDisplay = a.initialDisplay;
    isForDisplay = a.isForDisplay;

    dryMass = a.dryMass;
    coeffDrag = a.coeffDrag;
    dragArea = a.dragArea;
    srpArea = a.srpArea;
    reflectCoeff = a.reflectCoeff;
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

    // Duplicate the member data        
    epoch = a.epoch;
    dateFormat = a.dateFormat;
    stateType = a.stateType;
    refBody = a.refBody;
    refFrame = a.refFrame;
    refPlane = a.refPlane;

    for (int i = 0; i < 6; ++i)
    { 
       state[i] = a.state[i];
       displayState[i] = a.displayState[i];
    }
    displayEpoch = a.displayEpoch; 
    displayDateFormat = a.displayDateFormat; 
    displayCoordType = a.displayCoordType;
    initialDisplay = a.initialDisplay;
    isForDisplay = a.isForDisplay;

    dryMass = a.dryMass;
    coeffDrag = a.coeffDrag;
    dragArea = a.dragArea;
    srpArea = a.srpArea;
    reflectCoeff = a.reflectCoeff;

    return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Spacecraft.
 *
 * @return clone of the Spacecraft.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Spacecraft::Clone(void) const
{
   return (new Spacecraft(*this));
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
    if (str == "Epoch") return epochID;

    if (str == "DateFormat") return dateFormatID;

    if (str == "Element1" || str == "X" || str == "SMA" || str == "RMAG")  
       return state1ID;

    if (str == "Element2" || str == "Y" || str == "ECC" || str == "RA") 
       return state2ID;

    if (str == "Element3" || str == "Z" || str == "INC" || str == "DEC")
       return state3ID;

    if (str == "Element4" || str == "Vx" || str == "RAAN" || str == "VMAG") 
       return state4ID;

    if (str == "Element5" || str == "Vy" || str == "AOP" || str == "AZI" 
        || str == "RAV")
       return state5ID;

    if (str == "Element6" || str == "Vz" || str == "TA" || str == "FPA" 
        || str == "DECV") 
       return state6ID;

    if (str == "StateType") return stateTypeID;
    if (str == "ReferenceBody") return refBodyID;
    if (str == "ReferenceFrame") return refFrameID;
    if (str == "PrincipalPlane") return refPlaneID;

    // Drag Coefficient  
    if (str == "Cd") return coeffDragID;    

    // Reflectivity Coefficient
    if (str == "Cr") return reflectCoeffID;

    if (str == "DragArea") return dragAreaID;

    if (str == "DryMass") return dryMassID;

    if (str == "SRPArea") return srpAreaID;

    // Representation specific values
    if (str == "Position") {
        /// @todo: Force the representation to Cartesian -- later build 
        return state1ID;
    }
    if (str == "Velocity") {
        /// @todo: Force the representation to Cartesian -- later build 
        return state4ID;
    }

    return GmatBase::GetParameterID(str);
}


std::string Spacecraft::GetParameterText(const Integer id) const
{
    if (id == epochID) return "Epoch";

    if (id == dateFormatID) return "DateFormat";
  
    if (id == state1ID || id == state2ID || id == state3ID || id == state4ID
        || id == state5ID || id == state6ID)
        return GetElementName(id);

    if (id == stateTypeID) return "StateType";
    if (id == refBodyID) return "ReferenceBody";
    if (id == refFrameID) return "ReferenceFrame";
    if (id == refPlaneID) return "PrincipalPlane";

    if (id == dryMassID) return "DryMass";
    
    if (id == coeffDragID) return "Cd" ;

    if (id == dragAreaID) return "DragArea";

    if (id == srpAreaID) return "SRPArea";

    if (id == reflectCoeffID) return "Cr";

    return GmatBase::GetParameterText(id);
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
Gmat::ParameterType Spacecraft::GetParameterType(const Integer id) const
{
    if (id == epochID) return Gmat::REAL_TYPE;
    if (id == dateFormatID) return Gmat::STRING_TYPE;
    if (id == state1ID) return Gmat::REAL_TYPE;
    if (id == state2ID) return Gmat::REAL_TYPE;
    if (id == state3ID) return Gmat::REAL_TYPE;
    if (id == state4ID) return Gmat::REAL_TYPE;
    if (id == state5ID) return Gmat::REAL_TYPE;
    if (id == state6ID) return Gmat::REAL_TYPE;
    if (id == stateTypeID) return Gmat::STRING_TYPE;
    if (id == refBodyID) return Gmat::STRING_TYPE;
    if (id == refFrameID) return Gmat::STRING_TYPE;
    if (id == refPlaneID) return Gmat::STRING_TYPE;
    if (id == dryMassID) return Gmat::REAL_TYPE;
    if (id == coeffDragID) return Gmat::REAL_TYPE;
    if (id == dragAreaID) return Gmat::REAL_TYPE;
    if (id == srpAreaID) return Gmat::REAL_TYPE;
    if (id == reflectCoeffID) return Gmat::REAL_TYPE;
 
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
std::string Spacecraft::GetParameterTypeString(const Integer id) const
{
    return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
    if (id == epochID) return epoch;
    if (id == state1ID) return state[0];
    if (id == state2ID) return state[1];
    if (id == state3ID) return state[2];
    if (id == state4ID) return state[3];
    if (id == state5ID) return state[4];
    if (id == state6ID) return state[5];

    if (id == dryMassID) return dryMass;

    if (id == coeffDragID) return coeffDrag;
    if (id == dragAreaID) return dragArea;
    if (id == srpAreaID) return srpArea;
    if (id == reflectCoeffID) return reflectCoeff;

    return GmatBase::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <label> The label of the parameter.
 *
 * @return The parameter's value.
 */
Real Spacecraft::GetRealParameter(const std::string &label) const
{
    if (label == "Epoch") 
       return epoch;

    if (label == "Element1") return state[0];

    if (label == "Element2") return state[1];

    if (label == "Element3") return state[2];

    if (label == "Element4") return state[3];
 
    if (label == "Element5") return state[4];

    if (label == "Element6") return state[5];

    if (label == "Mass") return dryMass;
 
    if (label == "CoefficientDrag") return coeffDrag;
    if (label == "DragArea") return dragArea;
    if (label == "SRPArea") return srpArea;
    if (label == "ReflectivityCoefficient") return reflectCoeff;

    return GmatBase::GetRealParameter(label);
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
    if (id == epochID) return SetRealParameter("Epoch", value);

    // Check for the coordinate representation then set the value
    if (stateType == "Cartesian")
    {
       if (id == state1ID) return SetRealParameter("X",value); 
       if (id == state2ID) return SetRealParameter("Y",value); 
       if (id == state3ID) return SetRealParameter("Z",value); 
       if (id == state4ID) return SetRealParameter("Vx",value); 
       if (id == state5ID) return SetRealParameter("Vy",value); 
       if (id == state6ID) return SetRealParameter("Vz",value); 
    }
    else if (stateType == "Keplerian" || stateType == "ModifiedKeplerian") 
    {
       if (id == state1ID) 
       {
          if (stateType == "Keplerian")
             return SetRealParameter("SMA",value); 
          else
             return SetRealParameter("RadPer",value); 
       }

       if (id == state2ID) 
       {
          if (stateType == "Keplerian")
             return SetRealParameter("ECC",value); 
          else
             return SetRealParameter("RadApo",value); 
       }

       if (id == state3ID) return SetRealParameter("INC",value); 
       if (id == state4ID) return SetRealParameter("RAAN",value); 
       if (id == state5ID) return SetRealParameter("AOP",value); 
       if (id == state6ID) return SetRealParameter("TA",value); 
    }
    else if (stateType == "Spherical1" || stateType == "Spherical2")
    {
       if (id == state1ID) return SetRealParameter("RMAG",value); 
       if (id == state2ID) return SetRealParameter("RA",value); 
       if (id == state3ID) return SetRealParameter("DEC",value); 
       if (id == state4ID) return SetRealParameter("VMAG",value); 
       if (id == state5ID) 
       {
          if (stateType == "Spherical1")
             return SetRealParameter("AZI",value); 
          else 
             return SetRealParameter("RAV",value); 
       }   
       if (id == state6ID) 
       {
          if (stateType == "Spherical1")
             return SetRealParameter("FPA",value); 
          else 
             return SetRealParameter("DECV",value); 
       }   
    }

    if (id == dryMassID) return SetRealParameter("DryMass", value);

    if (id == coeffDragID) return SetRealParameter("Cd",value);
    if (id == dragAreaID) return SetRealParameter("DragArea",value);
    if (id == srpAreaID) return SetRealParameter("SRPArea",value);
    if (id == reflectCoeffID) 
        return SetRealParameter("Cr",value);

    return GmatBase::SetRealParameter(id, value);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <label> The label of the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or 
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the 
 *         parameter type is not Real.
 */
Real Spacecraft::SetRealParameter(const std::string &label, const Real value)
{
    if (label == "Epoch") return epoch = value;

    if (label == "X" || label == "SMA" || label == "RadPer" || label == "RMAG")
       return state[0] = value;

    if (label == "Y" || label == "ECC" || label == "RadApo" || label == "RA")  
       return state[1] = value;

    if (label == "Z" || label == "INC" || label == "DEC")  
       return state[2] = value;

    if (label == "Vx" || label == "RAAN" || label == "VMAG")  
       return state[3] = value;

    if (label == "Vy" || label == "AOP" || label == "AZI" || label == "RAV")  
       return state[4] = value;

    if (label == "Vz" || label == "TA" || label == "FPA" || label == "DECV")  
       return state[5] = value;

    if (label == "DryMass") return dryMass = value;

    if (label == "Cd") return coeffDrag = value;
    if (label == "DragArea") return dragArea = value;
    if (label == "SRPArea") return srpArea = value;
    if (label == "Cr") return reflectCoeff = value;

    return GmatBase::SetRealParameter(label, value);
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
    if (id == dateFormatID)
       return dateFormat;

    if (id == stateTypeID)
       return stateType; 

    if (id == refBodyID)
       return refBody; 

    if (id == refFrameID)
       return refFrame;

    if (id == refPlaneID)
       return refPlane;

    return GmatBase::GetStringParameter(id);
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
    if (id != dateFormatID && id != stateTypeID && id != refBodyID 
        && id != refFrameID && id != refPlaneID)
       return GmatBase::SetStringParameter(id, value);

    if (id == dateFormatID)
    {
       if (value != "TAIModJulian" && value != "UTCModJulian" && 
           value != "TAIGregorian" && value != "UTCGregorian")
       {
          return GmatBase::SetStringParameter(id, value);
       }
       dateFormat = value;
    }
    else if (id == stateTypeID)
    {  
       // Check for invalid input then return unknown value from GmatBase 
       if (value != "Cartesian" && value != "Keplerian" && 
           value != "ModifiedKeplerian" && value != "Spherical1" && 
           value != "Spherical2")
       {   
          return GmatBase::SetStringParameter(id, value);
       }
  
       stateType = value;
    }
    else if (id == refBodyID)
    {
       // Set the mu based on the reference body if no error
       if (stateConverter.SetMu(solarSystem,value))   
          refBody = value; 
       else
       {
          MessageInterface::ShowMessage("\n*** Warning ***\n%s (%lf)\n\n",
                "Spacecraft is not able to set mu so it uses original value:",
                 stateConverter.GetMu());
       }
    }
    else if (id == refFrameID)
    {  
       refFrame = value;
    }
    else 
       refPlane = value;

    return true;
}

//---------------------------------------------------------------------------
//  void SetEpoch()
//---------------------------------------------------------------------------
/**
 * Set the epoch.
 * 
 */
void Spacecraft::SetEpoch()
{
   // Check if date format is not TAIModJulian, then convert it
   if (displayDateFormat != "TAIModJulian")
   {
      try
      {
         displayEpoch = timeConverter.Convert(displayEpoch,displayDateFormat,
                                              "TAIModJulian");
         epoch = atof(displayEpoch.c_str());
      }
      catch (TimeConverter::TimeConverterException& e)
      {
         // do  nothing - retain with epoch
      }
   }
}

//---------------------------------------------------------------------------
//  Real* GetState()
//---------------------------------------------------------------------------
/**
 * Get the elements.
 * 
 * @return the state
 *
 */
Real* Spacecraft::GetState()
{
    return state;
}

//---------------------------------------------------------------------------
//  void SetState(const std::string elementType, Real *instate)
//---------------------------------------------------------------------------
/**
 * Set the elements to Cartesian states.
 * 
 * @param <elementType>  Element Type
 * @param <instate>      element states
 *
 */
void Spacecraft::SetState(const std::string &elementType, Real *instate)
{
    Rvector6 newState;

    newState.Set(instate[0],instate[1],instate[2],
                 instate[3],instate[4],instate[5]);

    if (elementType == "Keplerian")
    {
       newState = stateConverter.Convert(instate,elementType,"Cartesian");
    }
    else if (elementType == "ModifiedKeplerian")
    {
       // @todo - add converion from ModifiedKep to Cartesian
    }
    cartesianState = newState;

    SetState(newState.Get(0),newState.Get(1),newState.Get(2),
             newState.Get(3),newState.Get(4),newState.Get(5));
}

//---------------------------------------------------------------------------
//  void SetState(const Real s1, const Real s2, const Real s3, 
//                const Real s4, const Real s5, const Real s6)
//---------------------------------------------------------------------------
/**
 * Set the elements.
 * 
 * @param <s1>  First element
 * @param <s2>  Second element
 * @param <s3>  Third element
 * @param <s4>  Fourth element
 * @param <s5>  Fifth element
 * @param <s6>  Sixth element  
 *
 */
void Spacecraft::SetState(const Real s1, const Real s2, const Real s3, 
                          const Real s4, const Real s5, const Real s6)
{
    state[0] = s1;
    state[1] = s2;
    state[2] = s3;
    state[3] = s4;
    state[4] = s5;
    state[5] = s6;
}

//---------------------------------------------------------------------------
//  Rvector6 GetCartesianState() 
//---------------------------------------------------------------------------
/**
 * Get the converted Cartesian states from states in different coordinate type.
 * 
 * @return converted Cartesian states   
 *
 */
Rvector6 Spacecraft::GetCartesianState() 
{
   return(cartesianState = stateConverter.Convert(state,stateType,"Cartesian"));
}

//---------------------------------------------------------------------------
//  Rvector6 GetKeplerianState() 
//---------------------------------------------------------------------------
/**
 * Get the converted Keplerian states from states in different coordinate type.
 * 
 * @return converted Keplerain states   
 *
 */
Rvector6 Spacecraft::GetKeplerianState() 
{
   return(keplerianState = stateConverter.Convert(state,stateType,"Keplerian"));
}

//---------------------------------------------------------------------------
//  Rvector6 GetModifiedKeplerianState() 
//---------------------------------------------------------------------------
/**
 * Get the converted Modified Keplerian states from states in different 
 * coordinate type.
 * 
 * @return converted Modified Keplerain states   
 *
 */
Rvector6 Spacecraft::GetModifiedKeplerianState() 
{
   modifiedKeplerianState = 
      stateConverter.Convert(state,stateType,"ModifiedKeplerian");

#if 0
   // @todo- this won't work
   if (modifiedKeplerianState == NULL) 
   {
      MessageInterface::ShowMessage("Spacecraft::GetModifiedKeplerianState()\n"
                                    "Warning:  Unable to get Modified Keplerian"
                                    " state for the conversion so"); 
   }
#endif
   return (modifiedKeplerianState);
            
}

//---------------------------------------------------------------------------
//  bool GetDisplay() const
//---------------------------------------------------------------------------
/**
 * Get the display indicator 
 * 
 * @return display indicator 
 *
 */
bool Spacecraft::GetDisplay() const
{
   return isForDisplay;
}
 
//---------------------------------------------------------------------------
//  void SetDisplay(const bool displayFlag)
//---------------------------------------------------------------------------
/**
 * Set the display indicator 
 * 
 * @param  <displayFlag> display indicator 
 *
 */
void Spacecraft::SetDisplay(const bool displayFlag)
{
   isForDisplay = displayFlag;
}

//---------------------------------------------------------------------------
//  std::string SetDisplayDateFormat() const 
//---------------------------------------------------------------------------
/**
 * Get the display's dateformat of epoch.
 * 
 * @return date format. 
 *
 */
std::string Spacecraft::GetDisplayDateFormat() const 
{
   return displayDateFormat; 
}

//---------------------------------------------------------------------------
//  void SetDisplayDateFormat(const std::string &dateType) 
//---------------------------------------------------------------------------
/**
 * Set the display's dateformat of epoch.
 * 
 * @param <dateType> date type given. 
 *
 */
void Spacecraft::SetDisplayDateFormat(const std::string &dateType) 
{
   std::string tempType = dateType;

   if (initialDisplay)
      SetInitialDisplay();

   // Check if different coordinate type then convert the state
   if (displayDateFormat != dateType)
   {
      try
      {
         std::string newEpoch = timeConverter.Convert(displayEpoch,
                                displayDateFormat,dateType);
                                            
         SetDisplayEpoch(newEpoch);
      }
      catch (TimeConverter::TimeConverterException e)
      {
         // Stay with the original date format due to failure of conversion
         tempType = displayDateFormat;
      }
   }

   displayDateFormat = tempType;
}

//---------------------------------------------------------------------------
//  Real GetDisplayEpoch() const
//---------------------------------------------------------------------------
/**
 * Get the display's epoch.
 * 
 * @return display's epoch. 
 *
 */
std::string Spacecraft::GetDisplayEpoch()
{
    if (initialDisplay)
       SetInitialDisplay();

    return displayEpoch;
}

//---------------------------------------------------------------------------
//  void SetDisplayEpoch(const std::string &value) 
//---------------------------------------------------------------------------
/**
 * Set the display's epoch.
 * 
 * @param <value> Epoch input from GUI. 
 *
 */
void Spacecraft::SetDisplayEpoch(const std::string &value) 
{
    displayEpoch = value;
}

//---------------------------------------------------------------------------
//  std::string GetDisplayCoordType() const
//---------------------------------------------------------------------------
/**
 * Get the coordinate type of display's state.
 * 
 * @return coordintate type of display's state 
 *
 */
std::string Spacecraft::GetDisplayCoordType() const
{
   return displayCoordType;
}

//---------------------------------------------------------------------------
//  void SetDisplayCoordType(const std::string &coordType) 
//---------------------------------------------------------------------------
/**
 * Set the coordinate type of display's state.
 * 
 * @param <coordType> Coordintate type given. 
 *
 */
void Spacecraft::SetDisplayCoordType(const std::string &coordType) 
{
   if (initialDisplay)
      SetInitialDisplay();

   // Check if different coordinate type then convert the state
   if (displayCoordType != coordType)
   {
      Rvector6 newState = stateConverter.Convert(displayState,displayCoordType,
                                                coordType);
      SetDisplayState(newState);
   }

   displayCoordType = coordType;
}

//---------------------------------------------------------------------------
//  Real* GetDisplayState() 
//---------------------------------------------------------------------------
/**
 * Get the display's state.
 * 
 * @return display's state. 
 *
 */
Real* Spacecraft::GetDisplayState() 
{
    if (initialDisplay)
       SetInitialDisplay();

    return displayState;
}

//---------------------------------------------------------------------------
//  void SetDisplayState(const Real *s) 
//---------------------------------------------------------------------------
/**
 * Set the display's state.
 * 
 * @param <s> Input of state from GUI. 
 *
 */
void Spacecraft::SetDisplayState(const Real *s) 
{
    SetDisplayState(Rvector6(s[0],s[1],s[2],s[3],s[4],s[5]));
}

//---------------------------------------------------------------------------
//  void SetDisplayState(const Rvector6 s) 
//---------------------------------------------------------------------------
/**
 * Set the display's state.
 * 
 * @param <s> Input of state from GUI. 
 *
 */
void Spacecraft::SetDisplayState(const Rvector6 s) 
{
    for (int i=0; i < 6; i++)
         displayState[i] = s.Get(i);
}

//---------------------------------------------------------------------------
//  void SaveDisplay() 
//---------------------------------------------------------------------------
/**
 * Save the display state for updating the internal states.
 * 
 *
 */
void Spacecraft::SaveDisplay()
{
    SetEpoch(); 
    SetState(displayCoordType,displayState);
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystem()
//------------------------------------------------------------------------------
/**
 * Gets the solar system pointer
 *
 * @return Pointer to the solar system.
 */
//------------------------------------------------------------------------------
SolarSystem* Spacecraft::GetSolarSystem() const
{
   return solarSystem;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss Pointer to the solar system.
 */
//------------------------------------------------------------------------------
void Spacecraft::SetSolarSystem(SolarSystem *ss)
{
    // Get new value of mu; if fails, send out message and use old value 
    if (!stateConverter.SetMu(ss,refBody))
    {
       MessageInterface::ShowMessage("\n*** Warning ***\n%s (%lf)\n\n",
              "Spacecraft is not able to set Mu so it uses original value:",
              stateConverter.GetMu());
    } 

    solarSystem = ss;
}


//-------------------------------------
// private methods
//-------------------------------------

//---------------------------------------------------------------------------
//  void InitializeValues()
//---------------------------------------------------------------------------
/**
 * Initialize the default values of spacecraft information.
 *
 */
void Spacecraft::InitializeValues()
{
    epoch = Spacecraft::EPOCH; 

    state[0] = Spacecraft::ELEMENT1;
    state[1] = Spacecraft::ELEMENT2;
    state[2] = Spacecraft::ELEMENT3;
    state[3] = Spacecraft::ELEMENT4;
    state[4] = Spacecraft::ELEMENT5;
    state[5] = Spacecraft::ELEMENT6;

    dateFormat = Spacecraft::DATEFORMAT;
    stateType = Spacecraft::STATE_TYPE;
    refBody = Spacecraft::REF_BODY;
    refFrame = Spacecraft::REF_FRAME;
    refPlane = Spacecraft::REF_PLANE;
   
    subType = "Unused";

    dryMass = 850.0;
    coeffDrag = 2.2;
    dragArea = 15.0;
    srpArea = 1.0;
    reflectCoeff = 1.8;

    // Initialize non-internal states for display purpose 
    initialDisplay = true;
    isForDisplay = false;

    for (int i=0; i < 6; i++)
        displayState[i] = state[i];
 

    displayEpoch = ToString(epoch); 
    displayDateFormat = dateFormat; 
    displayCoordType = stateType;

    cartesianState.Set(state[0],state[1],state[2],state[3],state[4],state[5]); 
    
    keplerianState.Set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    // Initialize mu if it has SolarSystem; otherwise, use default value
    if (!stateConverter.SetMu(solarSystem,refBody))
    {
       MessageInterface::ShowMessage("\n*** Warning ***\n%s (%lf)\n\n",
              "Spacecraft is not able to set Mu so it uses original value:", 
              stateConverter.GetMu());
    }

}

//---------------------------------------------------------------------------
//  std::string GetElementName(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Get the string of element name 
 *
 * @param <id>   State id
 */
std::string Spacecraft::GetElementName(const Integer id) const
{
    // Get the local coordinate type
    std::string localCoordType = GetLocalCoordType();

    if (localCoordType == "Cartesian")
    {
       if (id == state1ID) return("X");  
       if (id == state2ID) return("Y");  
       if (id == state3ID) return("Z");  
       if (id == state4ID) return("Vx");  
       if (id == state5ID) return("Vy");  
       if (id == state6ID) return("Vz");  
    }
    else if (localCoordType == "Keplerian")
    {
       if (id == state1ID) return("SMA");  
       if (id == state2ID) return("ECC");  
       if (id == state3ID) return("INC");  
       if (id == state4ID) return("RAAN");  
       if (id == state5ID) return("AOP");  
       if (id == state6ID) 
       {
           // @todo will add subType to check with MA, TA, EA
           return("TA");  
       }
    }
    else if (localCoordType == "Spherical1" || localCoordType == "Spherical2")
    {
       if (id == state1ID) return("RMAG");  
       if (id == state2ID) return("RA");  
       if (id == state3ID) return("DEC");  
       if (id == state4ID) return("VMAG");  
       if (id == state5ID) 
       {
          if (localCoordType == "Spherical1")
             return("AZI");  
          else
             return("RAV");
       }
       if (id == state6ID) 
       {
          if (localCoordType == "Spherical1")
             return("FPA");  
          else
             return("DECV");
       }
    }
    else
    {
       if (id == state1ID) return("Element1");  
       if (id == state2ID) return("Element2");  
       if (id == state3ID) return("Element3");  
       if (id == state4ID) return("Element4");  
       if (id == state5ID) return("Element5");  
       if (id == state6ID) return("Element6");  
    }
    return("Unknown");
}

//---------------------------------------------------------------------------
//  std::string GetLocalCoordType() const
//---------------------------------------------------------------------------
/**
 * Get the local coordinate type based on the display indicator
 *
 * @return local coordinate type 
 */
std::string Spacecraft::GetLocalCoordType() const
{
    if (isForDisplay)
       return displayCoordType;
    else 
       return stateType;
}

//---------------------------------------------------------------------------
//  void SetInitialDisplay() 
//---------------------------------------------------------------------------
/**
 * Set the initial display's states from the internal epoch and states.
 * 
 *
 */
void Spacecraft::SetInitialDisplay() 
{
    for (int i=0; i < 6; i++)
         displayState[i] = state[i];

    displayEpoch = ToString(epoch);
    displayDateFormat = dateFormat;
    displayCoordType = stateType;
    initialDisplay = false;
}

//---------------------------------------------------------------------------
//  std::string ToString(const Real value) 
//---------------------------------------------------------------------------
/**
 * Set the initial display's states from the internal epoch and states.
 * 
 *
 */
std::string Spacecraft::ToString(const Real value)
{
    std::ostringstream valueBuffer;
    valueBuffer.precision(9);
    valueBuffer.setf(std::ios::fixed);
    valueBuffer << value;

    return valueBuffer.str();
}
