//$Header$
//------------------------------------------------------------------------------
//                                 Spacecraft
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
 * Definition of the Spacecraft class base
 */
//------------------------------------------------------------------------------

#ifndef Spacecraft_hpp
#define Spacecraft_hpp

#include "GmatBase.hpp"
#include "Cartesian.hpp"
#include "Keplerian.hpp"
#include "SphericalOne.hpp"
#include "SphericalTwo.hpp"

class GMAT_API Spacecraft : public GmatBase
{
public:
    // Default constructor
    Spacecraft();
    Spacecraft(const std::string &name);
    Spacecraft(const std::string &typeStr, const std::string &name);
    // Copy constructor
    Spacecraft(const Spacecraft &a);
    // Assignment operator
    Spacecraft& operator=(const Spacecraft &a);

    // Destructor
    virtual ~Spacecraft(void);

    // Parameter accessor methods -- overridden from GmatBase
    virtual Integer GetParameterID(const std::string &str) const;
    virtual Real GetRealParameter(const Integer id) const;
    virtual Real GetRealParameter(const std::string &label) const;
    virtual Real SetRealParameter(const Integer id, const Real value);
    virtual Real SetRealParameter(const std::string &label, const Real value);
    virtual std::string GetStringParameter(const Integer id) const;
    virtual bool SetStringParameter(const Integer id, const std::string &value);
    
    virtual std::string GetParameterText(const Integer id) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual Real* GetState(void); // { return state; }
    void SetState(Real s1, Real s2, Real s3, Real s4, Real s5, Real s6);

    void  ConvertRepresentation(const std::string &elementType);

    // Default values for spacecraft 
    static const Real EPOCH; 
    static const Real ELEMENT1; 
    static const Real ELEMENT2; 
    static const Real ELEMENT3; 
    static const Real ELEMENT4; 
    static const Real ELEMENT5; 
    static const Real ELEMENT6; 
    static const std::string REF_BODY; 
    static const std::string REF_FRAME; 
    static const std::string REF_PLANE; 

protected:
    // Declare protetced method data of spacecraft information
    Real         epoch;
    Real         state[6];  
    Real         mass;
    std::string  refBody; 
    std::string  refFrame;   
    std::string  refPlane; 
    Integer      epochID;
    Integer      state1ID;
    Integer      state2ID;
    Integer      state3ID;
    Integer      state4ID;
    Integer      state5ID;
    Integer      state6ID;       
    Integer      refBodyID; 
    Integer      refFrameID; 
    Integer      refPlaneID; 
    Integer      massID;

private:
    void InitializeValues();
    std::string GetElementName(const Integer id) const;
};

#endif // Spacecraft_hpp
