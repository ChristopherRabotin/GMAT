//$Header$
//------------------------------------------------------------------------------
//                                  Keplerian
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Rodger Abel, Wendy Shoan
// Created: 1995/07/18 for GSS project (originally KeplerianElements)
// Modified: 2003/09/16 Linda Jun - Added new methods and member data to return
//           data descriptions and values.
//
/**
 * Defines KeplerianElement class;  Performs conversions between mean, true and
 * eccentric anomaly.  Converts Cartesian Elements to Keplerian Elements.
 */
//------------------------------------------------------------------------------
#ifndef Keplerian_hpp
#define Keplerian_hpp

#include <iostream>
#include "gmatdefs.hpp"
#include "RealTypes.hpp"          // for Radians
#include "Linear.hpp"
#include "PhysicalConstants.hpp"  // for mu


// Forward declaration
class Cartesian;           

class Keplerian            
{
public :

    //  Default constructors and destructor and Copy constructor
    Keplerian();
    Keplerian(Real a, Real e, Radians i, Radians raan, Radians aop, Radians ma);
    Keplerian(const Keplerian &keplerian);
    const Keplerian & operator=(const Keplerian &keplerian);
    virtual ~Keplerian();

    //  functions to access member data
    Real GetSemimajorAxis() const;
    Real GetEccentricity() const;
    Radians GetInclination() const;
    Radians GetRAAscendingNode() const;
    Radians GetArgumentOfPeriapsis() const;
    Radians GetTrueAnomaly() const;
    Radians GetMeanAnomaly() const;

    //  functions to set member data
    void SetSemimajorAxis(Real a);
    void SetEccentricity(Real e);
    void SetInclination(Radians i);
    void SetRAAscendingNode(Radians raan);
    void SetArgumentOfPeriapsis(Radians aop);
    void SetTrueAnomaly(Radians ma);
    void SetMeanAnomaly(Radians ma);
    void SetAll(Real a, Real e, Radians i, Radians raan,
                Radians aop, Radians ma);

    // Conversion functions
    Cartesian ToCartesian(Real mu);

    //  Friend functions
    friend Radians MeanToEccentricAnomaly(Radians meanAnomaly,
                                          Real eccentricity);
    friend Radians TrueToEccentricAnomaly(Radians trueAnomaly,
                                          Real eccentricity);
    friend Radians EccentricToMeanAnomaly(Radians eccentricAnomaly,
                                          Real eccentricity);
    friend Radians TrueToMeanAnomaly(Radians trueAnomaly,
                                     Real eccentricity);
    friend Radians EccentricToTrueAnomaly(Radians eccentricAnomaly,
                                          Real eccentricity);
    friend Radians MeanToTrueAnomaly(Radians meanAnomaly,
                                     Real eccentricity);
    friend Keplerian ToKeplerian(const Cartesian &c,
                                 Real mu=GmatPhysicalConst::mu);

    friend std::ostream& operator<<(std::ostream& output, Keplerian& k);
    friend std::istream& operator>>(std::istream& input, Keplerian& k);

    Integer GetNumData() const;
    const std::string* GetDataDescriptions() const;
    std::string* ToValueStrings();

protected :

private :

    Real semimajorAxisD;         //  Default units are meters.  Can be
                                 //  input in KMs. if the user inputs
                                 //  the corresponding value of Mu,
                                 //  (gravitational constant).
    Real eccentricityD;
    Radians inclinationD;
    Radians RAAscendingNodeD;    //  right ascension of the orbit's
                                 //  ascending node.
    Radians argumentOfPeriapsisD;
    Radians meanAnomalyD;

   static const Integer NUM_DATA = 6;
    static const std::string DATA_DESCRIPTIONS[NUM_DATA];
    std::string stringValues[NUM_DATA];

};
#endif // Keplerian_hpp
