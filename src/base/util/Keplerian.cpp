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
// Modified: 2003/09/16 Linda Jun - See Keplerian.hpp
//
/**
 * Defines KeplerianElement class;  Performs conversions between mean, true and
 * eccentric anomaly.  Converts Cartesian Elements to Keplerian Elements.
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include "gmatdefs.hpp"
#include "RealUtilities.hpp" // for PI, TWO_PI, Sqrt()
#include "Linear.hpp"        // for I/O stream operations
#include "Cartesian.hpp"
#include "EphemerisUtil.hpp"
#include "Keplerian.hpp"
#include "Rvector.hpp"

using namespace GmatMathUtil;
using namespace GmatRealUtil;

//---------------------------------
// static data
//---------------------------------
const std::string Keplerian::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Semimajor Axis",
   "Eccentricity",
   "Inclination",
   "RA of Ascending Node",
   "Argument of Periapsis",
   "Mean Anomally"
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Keplerian::Keplerian() 
//------------------------------------------------------------------------------
Keplerian::Keplerian() 
{
}

//------------------------------------------------------------------------------
// Keplerian::Keplerian(Real a, Real e, Radians i,
//                      Radians raan, Radians aop, Radians m)
//------------------------------------------------------------------------------
Keplerian::Keplerian(Real a, Real e, Radians i,
                     Radians raan, Radians aop, Radians ma)
   :
   semimajorAxisD(a),
   eccentricityD(e),
   inclinationD(i),
   RAAscendingNodeD(raan),
   argumentOfPeriapsisD(aop),
   meanAnomalyD(ma)
{
}

//------------------------------------------------------------------------------
// Keplerian::Keplerian(const Keplerian &keplerian)
//------------------------------------------------------------------------------
Keplerian::Keplerian(const Keplerian &keplerian)
   :
   semimajorAxisD(keplerian.semimajorAxisD),
   eccentricityD(keplerian.eccentricityD),
   inclinationD(keplerian.inclinationD),
   RAAscendingNodeD(keplerian.RAAscendingNodeD),
   argumentOfPeriapsisD(keplerian.argumentOfPeriapsisD),
   meanAnomalyD(keplerian.meanAnomalyD)
{
}

//------------------------------------------------------------------------------
// const Keplerian& Keplerian::operator=(const Keplerian &keplerian)
//------------------------------------------------------------------------------
const Keplerian& Keplerian::operator=(const Keplerian &keplerian)
{
   if (this != &keplerian)
   {
      semimajorAxisD       = keplerian.semimajorAxisD;
      eccentricityD        = keplerian.eccentricityD;
      inclinationD         = keplerian.inclinationD;
      RAAscendingNodeD     = keplerian.RAAscendingNodeD;
      argumentOfPeriapsisD = keplerian.argumentOfPeriapsisD;
      meanAnomalyD         = keplerian.meanAnomalyD;
   }
   return *this;
}

//------------------------------------------------------------------------------
// Keplerian::~Keplerian() 
//------------------------------------------------------------------------------
Keplerian::~Keplerian() 
{
}

//------------------------------------------------------------------------------
// Real Keplerian::GetSemimajorAxis() const
//------------------------------------------------------------------------------
Real Keplerian::GetSemimajorAxis() const
{
   return semimajorAxisD;
}

//------------------------------------------------------------------------------
// Real Keplerian::GetEccentricity() const
//------------------------------------------------------------------------------
Real Keplerian::GetEccentricity() const
{
   return eccentricityD;
}

//------------------------------------------------------------------------------
// Radians Keplerian::GetInclination() const
//------------------------------------------------------------------------------
Radians Keplerian::GetInclination() const
{
   return inclinationD;
}

//------------------------------------------------------------------------------
// Radians Keplerian::GetRAAscendingNode() const
//------------------------------------------------------------------------------
Radians Keplerian::GetRAAscendingNode() const
{
   return RAAscendingNodeD;
}

//------------------------------------------------------------------------------
// Radians Keplerian:: GetArgumentOfPeriapsis() const
//------------------------------------------------------------------------------
Radians Keplerian:: GetArgumentOfPeriapsis() const
{
   return argumentOfPeriapsisD;
}

//------------------------------------------------------------------------------
// Radians Keplerian::GetMeanAnomaly() const
//------------------------------------------------------------------------------
Radians Keplerian::GetMeanAnomaly() const
{
   return meanAnomalyD;
}

//------------------------------------------------------------------------------
// Radians Keplerian::GetTrueAnomaly() const
//------------------------------------------------------------------------------
Radians Keplerian::GetTrueAnomaly() const
{
    return MeanToTrueAnomaly(meanAnomalyD, eccentricityD);
}

//------------------------------------------------------------------------------
// void Keplerian::SetSemimajorAxis(Real sma)
//------------------------------------------------------------------------------
void Keplerian::SetSemimajorAxis(Real a)
{
    semimajorAxisD = a;
}

//------------------------------------------------------------------------------
// void Keplerian::SetEccentricity(Real e)
//------------------------------------------------------------------------------
void Keplerian::SetEccentricity(Real e)
{
    eccentricityD = e;
}

//------------------------------------------------------------------------------
// void Keplerian::SetInclination(Radians i)
//------------------------------------------------------------------------------
void Keplerian::SetInclination(Radians i)
{
    inclinationD = i;
}

//------------------------------------------------------------------------------
// void Keplerian::SetRAAscendingNode(Radians raan)
//------------------------------------------------------------------------------
void Keplerian::SetRAAscendingNode(Radians raan)
{
    RAAscendingNodeD = raan;
}

//------------------------------------------------------------------------------
// void Keplerian::SetArgumentOfPeriapsis(Radians aop)
//------------------------------------------------------------------------------
void Keplerian::SetArgumentOfPeriapsis(Radians aop)
{
    argumentOfPeriapsisD = aop;
}

//------------------------------------------------------------------------------
// void Keplerian::SetMeanAnomaly(Radians ma)
//------------------------------------------------------------------------------
void Keplerian::SetMeanAnomaly(Radians ma)
{
    meanAnomalyD = ma;
}

//------------------------------------------------------------------------------
// void Keplerian::SetTrueAnomaly(Radians ta)
//------------------------------------------------------------------------------
void Keplerian::SetTrueAnomaly(Radians ta)
{
    meanAnomalyD = TrueToMeanAnomaly(ta, eccentricityD);
}

//------------------------------------------------------------------------------
// void Keplerian::Setall(Real a, Real e, Radians i, Radians raan,
//                                Radians aop, Radians ma)
//------------------------------------------------------------------------------
void Keplerian::SetAll(Real a, Real e, Radians i, Radians raan,
                       Radians aop, Radians ma)
{
   semimajorAxisD = a;
   eccentricityD = e;
   inclinationD = i;
   RAAscendingNodeD = raan;
   argumentOfPeriapsisD = aop;
   meanAnomalyD = ma;
}

//------------------------------------------------------------------------------
// Radians MeanToEccentricAnomaly(Radians meanAnomaly, Real eccentricity) 
//
// Notes:  Mean Anomaly is converted to eccentric Anomaly using Miles
//         Standish's formula; see GMAS subroutine description of
//         Celem for more information.
//------------------------------------------------------------------------------
Radians MeanToEccentricAnomaly(Radians meanAnomaly, Real eccentricity) 
{
    // Local variables
    Real d;
    Radians e1 = 0.0;
    Radians e2 = 0.0;
    Radians f;

    // Return value      
    Radians eccAnomaly;

    if (IsEqual(eccentricity, 0.0))
    {
        eccAnomaly = meanAnomaly;
    }
    else
    {
        e1 = meanAnomaly;
  
        for (int j = 0; j < EphemerisUtil::MAX_ITERATIONS; j++)
        {
            f  = e1 - Radians(eccentricity * Sin(e1)) - meanAnomaly;
            d  = 1.0 - eccentricity * Cos(e1 - 0.5 * f);
            e2 = e1 - (f / Radians(d));

            if (Abs(e1 - e2) < EphemerisUtil::KEPLER_TOLERANCE)
            {
                eccAnomaly = e2;

                if (eccAnomaly < 0.0)
                {
                    eccAnomaly += TWO_PI;
                }
                return eccAnomaly;
             }
             else
             { 
                 e1 = e2;
             }
        }  // end for loop

        // throw the exception, since conversion was not met within
        // MAX_ITERATIONS  
        throw EphemerisUtil::ToleranceNotMet();
    }
    return eccAnomaly;
}

//------------------------------------------------------------------------------
// Radians TrueToEccentricAnomaly(Radians trueAnomaly,
//                                Real eccentricity) 
//
// Notes: for information on how eccentricAnomaly is computed, see Methods 
//        of Orbit Determination by Pedro Ramon Escobal, pp. 118 - 119.
//------------------------------------------------------------------------------
Radians TrueToEccentricAnomaly(Radians trueAnomaly, Real eccentricity) 
{
    // Local variables
    const Real cosTrueAnomaly = Cos(trueAnomaly);
    const Real sinTrueAnomaly = Sin(trueAnomaly);
    const Real denom = 1.0 + eccentricity * cosTrueAnomaly;
    const Real cosEccAnomaly = (eccentricity + cosTrueAnomaly) / denom;
    const Real sinEccAnomaly = (Sqrt(1.0 - eccentricity * 
                                     eccentricity) * sinTrueAnomaly) / denom;
    Radians eccAnomaly;

    eccAnomaly = Mod(ATan(sinEccAnomaly,
                          cosEccAnomaly),
                          TWO_PI);

    // Make eccentric Anomaly a positive angle (0 - 2 pi)
    if (eccAnomaly < 0.0)
    {
        eccAnomaly += TWO_PI;
    }
    return eccAnomaly;
}

//------------------------------------------------------------------------------
// Radians EccentricToMeanAnomaly(Radians eccentricAnomaly,
//                                Real eccentricity)
//------------------------------------------------------------------------------
Radians EccentricToMeanAnomaly(Radians eccentricAnomaly,
                               Real eccentricity)
{
    return (eccentricAnomaly -
                   Radians(eccentricity * Sin(eccentricAnomaly)));
}

//------------------------------------------------------------------------------
// Radians TrueToMeanAnomaly(Radians trueAnomaly, Real eccentricity)
//------------------------------------------------------------------------------
Radians TrueToMeanAnomaly(Radians trueAnomaly, Real eccentricity)
{

    // Compute eccentric Anomaly
    if (IsEqual(eccentricity, 0.0))
    {
        return trueAnomaly;
    } 
    else
    {
        Radians eccentricAnomaly;
        eccentricAnomaly = TrueToEccentricAnomaly(trueAnomaly,
                                                  eccentricity);

        return EccentricToMeanAnomaly(eccentricAnomaly,
                                      eccentricity);
    }
}

//------------------------------------------------------------------------------
// Radians EccentricToTrueAnomaly(Radians eccentricAnomaly,
//                                Real eccentricity)
//
// Notes:  for information on how true anomaly is computed, see Methods
//         of Orbit Determination by Pedro Ramon Escobal, pp. 118 - 119
//------------------------------------------------------------------------------
Radians EccentricToTrueAnomaly(Radians eccentricAnomaly,
                               Real eccentricity)
{
    // Local variables
    const Real cosEccAnomaly = Cos(eccentricAnomaly);
    const Real sinEccAnomaly = Sin(eccentricAnomaly);
    const Real denom = 1.0 - eccentricity * cosEccAnomaly;
    const Real cosTrueAnomaly = (cosEccAnomaly - eccentricity) / denom;
    const Real sinTrueAnomaly = (Sqrt(1.0 - eccentricity * 
                                eccentricity) * sinEccAnomaly) / denom;
  
    // Return value
    Radians trueAnomaly;

    if (IsEqual(eccentricity, 0.0))
    {
        trueAnomaly = eccentricAnomaly;
    }
    else
    {
        trueAnomaly = ATan(sinTrueAnomaly,
                           cosTrueAnomaly);
    }

    // Make TrueAnomaly a positive angle (0 - 2 pi)
    if (trueAnomaly < 0.0)
    {
        trueAnomaly += TWO_PI;
    }
    return trueAnomaly;
} 

//------------------------------------------------------------------------------
// Radians MeanToTrueAnomaly(Radians meanAnomaly, Real eccentricity)
//------------------------------------------------------------------------------
Radians MeanToTrueAnomaly(Radians meanAnomaly, Real eccentricity)
{
    // Compute eccentric Anomaly
    if (IsEqual(eccentricity, 0.0))
    {
        return meanAnomaly;
    }
    else
    {
        Radians eccentricAnomaly;
        eccentricAnomaly = MeanToEccentricAnomaly(
                           meanAnomaly, eccentricity);

        return EccentricToTrueAnomaly(
               eccentricAnomaly,
               eccentricity);
    }
}

//------------------------------------------------------------------------------
// Keplerian ToKeplerian(const Cartesian &c, Real localMu)
//------------------------------------------------------------------------------
Keplerian ToKeplerian(const Cartesian &c, Real localMu)
{
    // Return value   
    Keplerian result;

    ToKeplerian(c.GetPosition(), c.GetVelocity(), localMu, 
                result.semimajorAxisD,
                result.eccentricityD,
                result.inclinationD,
                result.RAAscendingNodeD,
                result.argumentOfPeriapsisD,
                result.meanAnomalyD);

    return result;
}

//------------------------------------------------------------------------------
// std::ostream& operator<<(std::ostream& output, Keplerian& k)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, Keplerian& k)
{
    Rvector v(6, k.semimajorAxisD, k.eccentricityD,
              k.inclinationD,   k.RAAscendingNodeD,
              k.argumentOfPeriapsisD, k.meanAnomalyD);

    output << v << std::endl;

    return output;
}

//------------------------------------------------------------------------------
// std::istream& operator>>(std::istream& input, Keplerian& k)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, Keplerian& k)
{
    Rvector ke(6, 0.0);

    input >> ke;
    k.semimajorAxisD       = ke[0];
    k.eccentricityD        = ke[1];
    k.inclinationD         = ke[2];
    k.RAAscendingNodeD     = ke[3];
    k.argumentOfPeriapsisD = ke[4];
    k.meanAnomalyD         = ke[5];

    return input;
}

//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
Integer Keplerian::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Keplerian::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
// std::string* ToValueStrings()
//------------------------------------------------------------------------------
std::string* Keplerian::ToValueStrings()
{
   std::stringstream ss("");

   ss << semimajorAxisD;
   stringValues[0] = ss.str();
   
   ss.str("");
   ss << eccentricityD;
   stringValues[1] = ss.str();
   
   ss.str("");
   ss << inclinationD;
   stringValues[2] = ss.str();
   
   ss.str("");
   ss << RAAscendingNodeD;
   stringValues[3] = ss.str();
   
   ss.str("");
   ss << argumentOfPeriapsisD;
   stringValues[4] = ss.str();
   
   ss.str("");
   ss << meanAnomalyD;
   stringValues[5] = ss.str();

   return stringValues;
}

