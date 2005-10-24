//$Header$
//------------------------------------------------------------------------------
//                             Anomaly 
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Arthor:  Joey Gurganus 
// Created: 2005/02/17 
//
/**
 * Definition for the Anomaly class to compute the true, mean, and eccentric
 * anomaly using semi-major axis and eccentricity.
 *
 */
//------------------------------------------------------------------------------
#ifndef Anomaly_hpp
#define Anomaly_hpp

#include "gmatdefs.hpp"
#include "CoordUtil.hpp"
#include "RealUtilities.hpp"
#include "UtilityException.hpp"

class GMAT_API Anomaly 
{
public:
    Anomaly();
    Anomaly(const std::string &mType); 
    Anomaly(const Real a, const Real e, const Real value);
    Anomaly(const Real a, const Real e, const Real value, 
            const std::string &mType); 
    Anomaly(const Anomaly &anomaly);
    Anomaly& operator=(const Anomaly &anomaly);
    virtual ~Anomaly();

    // public method 
    void Set(const Real a, const Real e, const Real value, 
             const std::string &mType);

    Real GetSMA() const;
    void SetSMA(const Real a);

    Real GetECC() const;
    void SetECC(const Real e);

    Real GetValue() const;
    Real GetValue(const std::string &mType) const;
    void SetValue(const Real value);

    std::string GetType() const;
    void SetType(const std::string &t);

    Real GetTrueAnomaly() const;
    Real GetMeanAnomaly() const;
    Real GetEccentricAnomaly() const;

    bool IsInvalid(const std::string &inputType) const;

protected:
    static const Real    ANOMALY_TOL;// = 1.0e-30;
    static const Integer MAX_ITERATIONS = 75;

private:

    Real        semiMajorAxis;
    Real        eccentricity;
    Real        anomalyValue;
    std::string type;                 //  Anomaly type

};
#endif // Anomaly_hpp
