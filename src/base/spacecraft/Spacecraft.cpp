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

/// Set the default values for spacecraft 
const Real        Spacecraft::EPOCH      = 21545.0; 
const Real        Spacecraft::ELEMENT1 = 7100.0;
const Real        Spacecraft::ELEMENT2 = 0.0;
const Real        Spacecraft::ELEMENT3 = 1300.0; 
const Real        Spacecraft::ELEMENT4 = 0.0;
const Real        Spacecraft::ELEMENT5 = 7.35; 
const Real        Spacecraft::ELEMENT6 = 1.0;
const std::string Spacecraft::REF_BODY   = "Earth"; 
const std::string Spacecraft::REF_FRAME  = "Cartesian"; 
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
    refPlaneID   (parameterCount + 9),
    massID       (parameterCount + 10)
{
    // parameterCount += 10;
    parameterCount += 11;
    InitializeValues();
}

//---------------------------------------------------------------------------
//  Spacecraft(Gmat::ObjectTypes typeId, std::string &typeStr,
//             std::string &name)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 * @param <name> Optional name for the object.  Defaults to "".
 *
 */
Spacecraft::Spacecraft(const std::string &typeStr, const std::string &name) :
    GmatBase     (Gmat::SPACECRAFT, typeStr, name),
    epochID      (parameterCount),
    state1ID     (parameterCount + 1),
    state2ID     (parameterCount + 2),
    state3ID     (parameterCount + 3),
    state4ID     (parameterCount + 4),
    state5ID     (parameterCount + 5),
    state6ID     (parameterCount + 6),
    refBodyID    (parameterCount + 7),
    refFrameID   (parameterCount + 8),
    refPlaneID   (parameterCount + 9),
    massID       (parameterCount + 10)
{
    parameterCount += 11;
    InitializeValues();
}


// DJC: This method is in the header, but missing in the source
//---------------------------------------------------------------------------
//  Spacecraft(std::string &name)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 * @param <name> Optional name for the object.  Defaults to "".
 *
 */
Spacecraft::Spacecraft(const std::string &name) :
    GmatBase(Gmat::SPACECRAFT, "Spacecraft", name),
    epochID      (parameterCount),
    state1ID     (parameterCount + 1),
    state2ID     (parameterCount + 2),
    state3ID     (parameterCount + 3),
    state4ID     (parameterCount + 4),
    state5ID     (parameterCount + 5),
    state6ID     (parameterCount + 6),
    refBodyID    (parameterCount + 7),
    refFrameID   (parameterCount + 8),
    refPlaneID   (parameterCount + 9),
    massID       (parameterCount + 10)
{
    parameterCount += 11;
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
    GmatBase (a),
    epoch    (a.epoch),
    refBody  (a.refBody),
    refFrame (a.refFrame),
    refPlane (a.refPlane)
{
    for (int i = 0; i < 6; ++i)
       state[i] = a.state[i];

    mass = a.mass;
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

    if (str == "ReferenceBody") return refBodyID;
    if (str == "CoordinateRepresentation") return refFrameID;
    if (str == "PrincipalPlane") return refPlaneID;

    if (str == "Mass") return massID;

    // Representation specific values
    if (str == "Position") {
        /// @todo: Force the representation to Cartesian -- build 2
        return state1ID;
    }
    if (str == "Velocity") {
        /// @todo: Force the representation to Cartesian -- build 2
        return state4ID;
    }

    return GmatBase::GetParameterID(str);
}


std::string Spacecraft::GetParameterText(const Integer id) const
{
    if (id == epochID) return "Epoch";
  
    if (id == state1ID || id == state2ID || id == state3ID || id == state4ID
        || id == state5ID || id == state6ID)
        return GetElementName(id);

    if (id == refBodyID) return "ReferenceBody";
    if (id == refFrameID) return "CoordinateRepresentation";
    if (id == refPlaneID) return "PrincipalPlane";
    if (id == massID) return "Mass";
    
    return GmatBase::GetParameterText(id);
}

Gmat::ParameterType Spacecraft::GetParameterType(const Integer id) const
{
    if (id == epochID) return Gmat::REAL_TYPE;
    if (id == state1ID) return Gmat::REAL_TYPE;
    if (id == state2ID) return Gmat::REAL_TYPE;
    if (id == state3ID) return Gmat::REAL_TYPE;
    if (id == state4ID) return Gmat::REAL_TYPE;
    if (id == state5ID) return Gmat::REAL_TYPE;
    if (id == state6ID) return Gmat::REAL_TYPE;
    if (id == refBodyID) return Gmat::STRING_TYPE;
    if (id == refFrameID) return Gmat::STRING_TYPE;
    if (id == refPlaneID) return Gmat::STRING_TYPE;
    if (id == massID) return Gmat::REAL_TYPE;
    
    return GmatBase::GetParameterType(id);
}

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

    if (id == massID) return mass;
    
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

    if (label == "Element1" || label == "X" || label == "SMA"  ||
        label == "RMAG")
       return state[0];

    if (label == "Element2" || label == "Y" || label == "ECC" || label == "RA") 
       return state[1];

    if (label == "Element3" || label == "Z" || label == "INC" || label == "DEC")
       return state[2];

    if (label == "Element4" || label == "Vx" || label == "RAAN" || 
        label == "VMAG") 
       return state[3];
 
    if (label == "Element5" || label == "Vy" || label == "AOP" || 
        label == "AZI" || label == "RAV")
       return state[4];

    if (label == "Element6" || label == "Vz" || label == "TA" || 
        label == "FPA" || label == "DECV")
       return state[5];

    if (label == "Mass")
       return mass;
 
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
    if (refFrame == "Cartesian")
    {
       if (id == state1ID) return SetRealParameter("X",value); 
       if (id == state2ID) return SetRealParameter("Y",value); 
       if (id == state3ID) return SetRealParameter("Z",value); 
       if (id == state4ID) return SetRealParameter("Vx",value); 
       if (id == state5ID) return SetRealParameter("Vy",value); 
       if (id == state6ID) return SetRealParameter("Vz",value); 
    }
    else if (refFrame == "Keplerian")
    {
       if (id == state1ID) return SetRealParameter("SMA",value); 
       if (id == state2ID) return SetRealParameter("ECC",value); 
       if (id == state3ID) return SetRealParameter("INC",value); 
       if (id == state4ID) return SetRealParameter("RAAN",value); 
       if (id == state5ID) return SetRealParameter("AOP",value); 
       if (id == state6ID) return SetRealParameter("TA",value); 
    }
    else if (refFrame == "Spherical1" || refFrame == "Spherical2")
    {
       if (id == state1ID) return SetRealParameter("RMAG",value); 
       if (id == state2ID) return SetRealParameter("RA",value); 
       if (id == state3ID) return SetRealParameter("DEC",value); 
       if (id == state4ID) return SetRealParameter("VMAG",value); 
       if (id == state5ID) 
       {
          if (refFrame == "Spherical1")
             return SetRealParameter("AZI",value); 
          else 
             return SetRealParameter("RAV",value); 
       }   
       if (id == state6ID) 
       {
          if (refFrame == "Spherical1")
             return SetRealParameter("FPA",value); 
          else 
             return SetRealParameter("DECV",value); 
       }   
    }

    if (id == massID) return SetRealParameter("Mass", value);

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

    if (label == "X" || label == "SMA" || label == "RMAG")  
       return state[0] = value;

    if (label == "Y" || label == "ECC" || label == "RA")  
       return state[1] = value;

    if (label == "Z" || label == "INC" || label == "DEC")  
       return state[2] = value;

    if (label == "Vx" || label == "RAAN" || label == "VMAG")  
       return state[3] = value;

    if (label == "Vy" || label == "AOP" || label == "AZI" || label == "RAV")  
       return state[4] = value;

    if (label == "Vz" || label == "TA" || label == "FPA" || label == "DECV")  
       return state[5] = value;

    if (label == "Mass") return mass = value;

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
    if (id != refBodyID && id != refFrameID && id != refPlaneID)
       return GmatBase::SetStringParameter(id, value);

    if (id == refBodyID)
       refBody = value; 
    else if (id == refFrameID)
    {  
       // Check for invalid input then return unknown value from GmatBase 
       if (value != "Cartesian" && value != "Keplerian" && 
           value != "Spherical1" && value != "Spherical2")
       {   
          return GmatBase::SetStringParameter(id, value);
       }
  
       refFrame = value;
    }
    else 
       refPlane = value;

    return true;
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
//  void SetState(Real s1, Real s2, Real s3, Real s4, Real s5, Real s6)
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
void Spacecraft::SetState(Real s1, Real s2, Real s3, Real s4, Real s5, Real s6)
{
    state[0] = s1;
    state[1] = s2;
    state[2] = s3;
    state[3] = s4;
    state[4] = s5;
    state[5] = s6;
}

//---------------------------------------------------------------------------
//  void Spacecraft::ConvertRepresentation(const std::string &elementType)
//---------------------------------------------------------------------------
/**
 * Converts to the elements' coordinate respresentation.
 * 
 * @param <elementType>  Type of coordinate system 
 *
 */
void Spacecraft::ConvertRepresentation(const std::string &elementType)
{
    // Determine the input of coordinate representation 
    if (refFrame == "Cartesian")
    {
       if (elementType == "Cartesian")
          return;  // do nothing  
       
       Cartesian *cartesian = new Cartesian(state[0],state[1],state[2],
                                            state[3],state[4],state[5]);
       if (elementType == "Keplerian")
       {
          Keplerian *keplerian = new Keplerian(ToKeplerian(*cartesian,
                                               GmatPhysicalConst::mu));
         
          // Set the spacecraft's elements
          SetState(keplerian->GetSemimajorAxis(),
                   keplerian->GetEccentricity(),
                   keplerian->GetInclination(),
                   keplerian->GetRAAscendingNode(),
                   keplerian->GetArgumentOfPeriapsis(),
                   keplerian->GetMeanAnomaly() );
                   
          delete keplerian;           
       }
       else if (elementType == "Spherical1")
       {
          SphericalOne *spherical = new SphericalOne();
          if (spherical->ToSphericalOne(*cartesian))
          {
             // Set the spacecraft's elements
             SetState(spherical->GetRadicalMagnitude(),
                      spherical->GetRightAscension(),
                      spherical->GetDeclination(),
                      spherical->GetVelocityMagnitude(),
                      spherical->GetAzimuth(),
                      spherical->GetFlightPathAngle());
          }
          else
          {
             SetState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
          }
          delete spherical;
       }
       else if (elementType == "Spherical2")
       {
          SphericalTwo *spherical = new SphericalTwo();
          if (spherical->ToSphericalTwo(*cartesian)) 
          {
             // Set the spacecraft's elements
             SetState(spherical->GetRadicalMagnitude(),
                      spherical->GetRightAscension(),
                      spherical->GetDeclination(),
                      spherical->GetVelocityMagnitude(),
                      spherical->GetVelocityRA(),
                      spherical->GetVelocityDeclination());
          }
          else
          {
             SetState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
          }
          delete spherical;

       }
       delete cartesian;
    }
    else if (refFrame == "Keplerian")
    {
       if (elementType == "Keplerian")
          return;   // do nothing

       Keplerian *keplerian = new Keplerian(state[0],state[1],state[2],
                                            state[3],state[4],state[5]);
   
       Cartesian *cartesian = new Cartesian(ToCartesian(*keplerian,
                                               GmatPhysicalConst::mu));

       delete keplerian;

       if (elementType == "Cartesian")
       {
          // Get the position and velocity from Cartesian
          Rvector3 position = cartesian->GetPosition();
          Rvector3 velocity = cartesian->GetVelocity();

          // Set the spacecraft's elements
          SetState(position.Get(0),position.Get(1),position.Get(2),
                   velocity.Get(0),velocity.Get(1),velocity.Get(2));
       }
       else if (elementType == "Spherical1")
       {
          SphericalOne *spherical = new SphericalOne();
          if (spherical->ToSphericalOne(*cartesian)) 
          {
             // Set the spacecraft's elements
             SetState(spherical->GetRadicalMagnitude(),
                      spherical->GetRightAscension(),
                      spherical->GetDeclination(),
                      spherical->GetVelocityMagnitude(),
                      spherical->GetAzimuth(),
                      spherical->GetFlightPathAngle());
          }
          else
          {
             SetState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
          }
          delete spherical;
              
       }
       else if (elementType == "Spherical2")
       {
          SphericalTwo *spherical = new SphericalTwo();
          if (spherical->ToSphericalTwo(*cartesian)) 
          {
             // Set the spacecraft's elements
             SetState(spherical->GetRadicalMagnitude(),
                      spherical->GetRightAscension(),
                      spherical->GetDeclination(),
                      spherical->GetVelocityMagnitude(),
                      spherical->GetVelocityRA(),
                      spherical->GetVelocityDeclination());
          }
          else
          {
             SetState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
          }
          delete spherical;
       }
       delete cartesian;
    }
    else if (refFrame == "Spherical1")
    {
       if (elementType == "Spherical1")
          return;  // do nothing
  
       SphericalOne *spherical = new SphericalOne(state[0],state[1],state[2],
                                                  state[3],state[4],state[5]);

       Cartesian *cartesian = new Cartesian(spherical->GetCartesian()); 

       if (elementType == "Cartesian")
       {
           // Get the position and velocity from Cartesian
           Rvector3 position = cartesian->GetPosition();
           Rvector3 velocity = cartesian->GetVelocity();
 
          // Set the spacecraft's elements
          SetState(position.Get(0),position.Get(1),position.Get(2),
                   velocity.Get(0),velocity.Get(1),velocity.Get(2));
       }
       else if (elementType == "Keplerian")
       {
          Keplerian *keplerian = new Keplerian(ToKeplerian(*cartesian,
                                               GmatPhysicalConst::mu));

          // Set the spacecraft's elements
          SetState(keplerian->GetSemimajorAxis(),
                   keplerian->GetEccentricity(),
                   keplerian->GetInclination(),
                   keplerian->GetRAAscendingNode(),      
                   keplerian->GetArgumentOfPeriapsis(),
                   keplerian->GetMeanAnomaly() );

          delete keplerian;
       }
       delete cartesian;
       delete spherical;
    }
    else if (refFrame == "Spherical2")
    {
       if (elementType == "Spherical2")
          return;  // do nothing

       SphericalTwo *spherical = new SphericalTwo(state[0],state[1],state[2],
                                                 state[3],state[4],state[5]);

       Cartesian *cartesian = new Cartesian(spherical->GetCartesian()); 

       if (elementType == "Cartesian")
       {
          // Get the position and velocity from Cartesian
          Rvector3 position = cartesian->GetPosition();
          Rvector3 velocity = cartesian->GetVelocity();

          // Set the spacecraft's elements
          SetState(position.Get(0),position.Get(1),position.Get(2),
                   velocity.Get(0),velocity.Get(1),velocity.Get(2));

       }
       else if (elementType == "Keplerian")
       {
          Keplerian *keplerian = new Keplerian(ToKeplerian(*cartesian,
                                               GmatPhysicalConst::mu));

          // Set the spacecraft's elements
          SetState(keplerian->GetSemimajorAxis(),
                   keplerian->GetEccentricity(),
                   keplerian->GetInclination(),
                   keplerian->GetRAAscendingNode(),      
                   keplerian->GetArgumentOfPeriapsis(),
                   keplerian->GetMeanAnomaly() );

          delete keplerian;
       }
       delete cartesian;
       delete spherical;
    }
    else
       return;  // do nothing...

    refFrame = elementType;
}

//-------------------------------------
// private methods
//-------------------------------------

//---------------------------------------------------------------------------
//  void InitializeValues()
//---------------------------------------------------------------------------
/**
 * Initialize the default values of spacecraft including epoch, semi-major axis,
 * eccentricity, inclination, right ascension of ascending node, argument of
 * perigee, and mean anomaly.
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

    refBody = Spacecraft::REF_BODY;
    refFrame = Spacecraft::REF_FRAME;
    refPlane = Spacecraft::REF_PLANE;

    mass = 0.0;
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
    if (refFrame == "Cartesian")
    {
       if (id == state1ID) return("X");  
       if (id == state2ID) return("Y");  
       if (id == state3ID) return("Z");  
       if (id == state4ID) return("Vx");  
       if (id == state5ID) return("Vy");  
       if (id == state6ID) return("Vz");  
    }
    else if (refFrame == "Keplerian")
    {
       if (id == state1ID) return("SMA");  
       if (id == state2ID) return("ECC");  
       if (id == state3ID) return("INC");  
       if (id == state4ID) return("RAAN");  
       if (id == state5ID) return("AOP");  
       if (id == state6ID) return("MA");  
    }
    else if (refFrame == "Spherical1" || refFrame == "Spherical2")
    {
       if (id == state1ID) return("RMAG");  
       if (id == state2ID) return("RA");  
       if (id == state3ID) return("DEC");  
       if (id == state4ID) return("VMAG");  
       if (id == state5ID) 
       {
          if (refFrame == "Spherical1")
             return("AZI");  
          else
             return("RAV");
       }
       if (id == state6ID) 
       {
          if (refFrame == "Spherical1")
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
