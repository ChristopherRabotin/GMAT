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


class Burn : public GmatBase{
public:
	Burn(std::string typeStr, std::string nomme);
	virtual ~Burn();
    Burn(const Burn &b);
    Burn            operator=(const Burn &b);
    
    // Inherited (GmatBase) methods
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer     GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual Real        GetRealParameter(const Integer id) const;
    virtual Real        SetRealParameter(const Integer id,
                                         const Real value);
    virtual std::string GetStringParameter(const Integer id) const;
    virtual bool        SetStringParameter(const Integer id, 
                                           const std::string &value);
    
protected:
    /// Text description of the coordinate frame type -- e.g. VNB or LVLH
    std::string         coordFrame;
    /// Text description of the coordinate system -- e.g. Cartesian or Spherical
    std::string         coordSystem;
    /// Orientation vector for maneuver; includes magnitude for impulsive burns
    Real                deltaV[3];
    /// Common string names for the 3 components
    std::string         dvLabels[3];
    
    // Parameter ID mappings
    /// ID for the coordinate frame string
    const Integer           coordFrameID;
    /// ID for the coordinate frame string
    const Integer           coordSystemID;
    /// ID for the coordinate frame string
    const Integer           deltaV1ID;
    /// ID for the coordinate frame string
    const Integer           deltaV2ID;
    /// ID for the coordinate frame string
    const Integer           deltaV3ID;

    /// ID for the coordinate frame string
    const Integer           deltaV1LabelID;
    /// ID for the coordinate frame string
    const Integer           deltaV2LabelID;
    /// ID for the coordinate frame string
    const Integer           deltaV3LabelID;
};


#endif // Burn_hpp
