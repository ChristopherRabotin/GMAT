//$Header$
//------------------------------------------------------------------------------
//                                   Burn
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
 * Defines the Burn base class used for impulsive and finite maneuvers. 
 */
//------------------------------------------------------------------------------


#ifndef Burn_hpp
#define Burn_hpp


#include "GmatBase.hpp"
#include "BurnException.hpp"
#include "ManeuverFrame.hpp"
#include "ManeuverFrameManager.hpp"
#include "Spacecraft.hpp"


/**
 * All maneuver classes are derived from this base class.
 */
class Burn : public GmatBase
{
public:
   Burn(const std::string &typeStr, const std::string &nomme);
   virtual ~Burn();
   Burn(const Burn &b);
   Burn&                   operator=(const Burn &b);
    
    // Inherited (GmatBase) methods
    virtual std::string     GetParameterText(const Integer id) const;
    virtual Integer         GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                            GetParameterType(const Integer id) const;
    virtual std::string     GetParameterTypeString(const Integer id) const;

    virtual Real            GetRealParameter(const Integer id) const;
    virtual Real            SetRealParameter(const Integer id,
                                         const Real value);
    virtual std::string     GetStringParameter(const Integer id) const;
    virtual bool            SetStringParameter(const Integer id, 
                                           const std::string &value);
    virtual const StringArray&    
                            GetStringArrayParameter(const Integer id) const;
    
    // Accessor method used by Maneuver to pass in the spacecraft pointer
    void                    SetSpacecraftToManeuver(Spacecraft *sat);

    /**
     * Applies the burn.  
     * 
     * Derived classes implement this method to provide the mathematics that 
     * model the burn.  The parameter is provided so that the derived classes 
     * have an interface to pass in additional data is needed.
     * 
     * @param <burnData>    Array of data specific to the derived burn class. 
     */
    virtual bool            Fire(Real *burnData = NULL) = 0;
    
protected:
    /// Text description of the coordinate frame type -- e.g. VNB or LVLH
    std::string             coordFrame;
    /// Text description of the coordinate system -- e.g. Cartesian or Spherical
    std::string             coordSystem;
    /// Orientation vector for maneuver; includes magnitude for impulsive burns
    Real                    deltaV[3];
    /// Common string names for the 3 components
    std::string             dvLabels[3];
    /// Maneuver frame conversion class manager
    ManeuverFrameManager    *frameman;
    /// Current maneuver frame
    ManeuverFrame           *frame;
    /// Matrix of maneuver frame vectors
    Real                    frameBasis[3][3];
    /// Name of the Spacecraft that gets maneuvered
    std::string             satName;
    /// Pointer to the spacecraft that maneuvers
    Spacecraft              *sc;
    
    // Parameter ID mappings
    /// ID for the coordinate frame string
    const Integer           coordFrameID;
    /// ID for the coordinate system string
    const Integer           coordSystemID;
    /// ID for the "x" component of the maneuver
    const Integer           deltaV1ID;
    /// ID for the "y" component of the maneuver
    const Integer           deltaV2ID;
    /// ID for the "z" component of the maneuver
    const Integer           deltaV3ID;
    /// ID for the string label for the "x" component
    const Integer           deltaV1LabelID;
    /// ID for the string label for the "y" component
    const Integer           deltaV2LabelID;
    /// ID for the string label for the "z" component
    const Integer           deltaV3LabelID;
    /// ID for the spacecraft that is maneuvered
    const Integer           satNameID;
};


#endif // Burn_hpp
