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

class GMAT_API Spacecraft : public GmatBase
{
public:
    // Default constructor
    Spacecraft();
    Spacecraft(const std::string &nomme);
    Spacecraft(const std::string &typeStr, const std::string &nomme);
    // Copy constructor
    Spacecraft(const Spacecraft &a);
    // Assignment operator
    Spacecraft& operator=(const Spacecraft &a);

    // Destructor
    virtual ~Spacecraft(void);

    // Parameter accessor methods -- overridden from GmatBase
    virtual Integer GetParameterID(const std::string &str) const;
    virtual Real GetRealParameter(const Integer id) const;
    virtual Real SetRealParameter(const Integer id, const Real value);
    virtual std::string GetStringParameter(const Integer id) const;
    virtual bool SetStringParameter(const Integer id, const std::string &value);
    
    virtual std::string GetParameterText(const Integer id) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    virtual Real* GetState(void)
    { return state; }

    
    // Default values for spacecraft 
    static const Real EPOCH; 
    static const Real SEMI_MAJOR_AXIS; 
    static const Real ARG_OF_PERIGEE; 
    static const Real INCLINATION; 
    static const std::string REF_BODY; 
    static const std::string REF_FRAME; 
    static const std::string REF_PLANE; 

protected:
    // Declare protetced method data of spacecraft information
    Real         epoch;
    Real         state[6];  // state[0]: semi-major axis (km)
                            // state[1]: eccentricity
                            // state[2]: inclination (rad) 
                            // state[3]: right ascension of ascending node (rad)
                            // state[4]: argument of perigee (rad)
                            // state[5]: mean anomaly (rad)
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

private:

};

#endif // Spacecraft_hpp
