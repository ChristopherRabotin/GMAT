//$Header$
//------------------------------------------------------------------------------
//                             EphemerisUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Rodger Abel
// Created: 1995/07/18 for GSS project.
// Modified:
//   2003/09/16 Linda Jun - Reused for GMAT project
//
/**
 * Defines ephemeris exceptions and constants and conversions between
 * Cartesian and Keplerian elements.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "EphemerisUtil.hpp"
#include "RealTypes.hpp"      // for RealConst
#include "Rvector3.hpp"
#include "RealUtilities.hpp"  // for Sqrt(), IsZero()
#include "Keplerian.hpp"

using namespace GmatMathUtil;

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Keplerian ToKeplerian(const Rvector3 &position, const Rvector3 &velocity,
//                       Real mu)
//------------------------------------------------------------------------------
Keplerian ToKeplerian(const Rvector3 &position, const Rvector3 &velocity,
                      Real mu)
{
   Real sma, ecc, inc, raan, aop, ma;
   ToKeplerian(position, velocity, mu, sma, ecc, inc, raan, aop, ma);
   return Keplerian(sma, ecc, inc, raan, aop, ma);
}

//------------------------------------------------------------------------------
//  <friend>
//  void ToKeplerian(const Rvector3 &position, const Rvector3 &velocity,
//                   Real mu, Real &sma, Real &ecc, Real &inc,
//                   Real &raan, Real &aop, Real &ma)
//------------------------------------------------------------------------------
void ToKeplerian(const Rvector3 &position, const Rvector3 &velocity,
                 Real mu, Real &sma, Real &ecc, Real &inc,
                 Real &raan, Real &aop, Real &ma)
{
    Real eSquared;
    const Rvector3 h = Cross(position, velocity);
    const Real rSquared = position[0]*position[0] + 
                          position[1]*position[1] + position[2]*position[2];
    const Real r = GmatMathUtil::Sqrt(rSquared);             //  Position Magnitude

    const Real vSquared = velocity[0]*velocity[0] + 
                          velocity[1]*velocity[1] + velocity[2]*velocity[2];
    const Real v = GmatMathUtil::Sqrt(vSquared);             //  Velocity Magnitude

    const Real rDotV = position * velocity;

    //  Semilatus Rectum
    const Real p = (rSquared * vSquared - (rDotV * rDotV)) / mu;

    const Real sqRootMuP = GmatMathUtil::Sqrt(mu * p);

    Radians argLat;
    Radians eccAnomaly;
    Real muA;
    Real sqRootMuA;
    Radians trueAnomaly;

    //  If position or velocity magnitude are zero, return all zeros
    if ((GmatMathUtil::IsEqual(r, 0.0)) || (GmatMathUtil::IsEqual(v, 0.0)))
    {
        sma  = 0.0;
        ecc  = 0.0;
        inc  = 0.0;
        raan = 0.0;
        aop  = 0.0;
        ma   = 0.0;
        return;
    }

    //  Compute semimajor Axis (A)
    sma = (mu * r) / (2.0 * mu - r * vSquared);

    if (sma <= 0.0)
    {
        throw EphemerisUtil::InvalidEllipticalElements();
    }

    //  Compute eccentricity;
    eSquared = 1.0 - p/sma;

    if (eSquared < -GmatRealConst::REAL_EPSILON)
    {
        throw EphemerisUtil::InvalidEllipticalElements();
    }
    else if (eSquared < 0.0)
    {
        ecc = 0.0;
    }
    else
    {
        ecc = GmatMathUtil::Sqrt(eSquared);
    }

    if (ecc >= (1.0 + GmatRealConst::REAL_EPSILON))
    {
        throw EphemerisUtil::InvalidEllipticalElements();
    }
    else if (ecc > 1.0)
    {
        ecc = 1.0;
    }

    //  Compute inclination
    inc = ATan(GmatMathUtil::Sqrt(h[0]*h[0] + h[1]*h[1]), h[2]);

    //  Compute the Right Ascension of the Ascending Node (RA)
    if (GmatMathUtil::IsEqual(inc, 0.0))
    {
        raan = 0.0;
        if (GmatMathUtil::IsEqual(position[2], r))
        {
            argLat = PI_OVER_TWO;
        }
        else
        {
            argLat = ATan(position[1], position[0]);
        }
    }
    else
    {
        raan = ATan(h[0], -h[1]);
        raan = Mod(raan, TWO_PI);
        argLat = ATan(position[2], Sin(inc) * (position[0]*Cos(raan) + 
                 position[1]* Sin(raan)));
    }

    if (argLat < 0.0)
    {
        argLat += TWO_PI;
    }

    //  Make Right Ascension of Ascending Node positive
    if (raan < 0.0)
    {
        raan += TWO_PI;
    }

    //  Compute the argument of Periapsis (argPer)
    if (GmatMathUtil::IsEqual(ecc, 0.0))
    {
        trueAnomaly = argLat;
    }
    else
    {
       trueAnomaly = ATan( (rDotV * sqRootMuP),
                           (mu * (p-r)));
       trueAnomaly = Mod(trueAnomaly, TWO_PI);

       if (trueAnomaly < 0.0)
       {
           trueAnomaly += TWO_PI;
       }
    }
    aop = argLat - trueAnomaly;

    //  Make argument of Periapsis positive
    if (aop < 0.0)
    {
        aop += TWO_PI;
    }

    //  Compute Mean Anomaly
    if (GmatMathUtil::IsEqual(ecc, 0.0))
    {
        ma = argLat;
    }
    else
    {
        //  Compute eccentric Anomaly (eccAnomaly)
        muA = mu * sma;

        if (muA < 0.0)
        {
            sma   = 0.0;
            ecc   = 0.0;
            inc   = 0.0;
            raan  = 0.0;
            aop   = 0.0;
            ma   = 0.0;
            return;
        }

        sqRootMuA   = GmatMathUtil::Sqrt(muA);
        eccAnomaly  = ATan((rDotV / sqRootMuA), (1.0 - (r/sma)));
        eccAnomaly  = Mod(eccAnomaly, TWO_PI);

        //  Make eccentric Anomaly positive
        if (eccAnomaly < 0.0)
        {
            eccAnomaly += TWO_PI;
        }

        //  Compute Mean Anomaly
        ma = eccAnomaly - Radians(rDotV / sqRootMuA);

        //  Make Mean Anomaly positive
        if (ma < 0.0)
        {
            ma += TWO_PI;
        }

    }
}

//------------------------------------------------------------------------------
//  <friend>
//  void ToCartesian(Real sma, Real ecc, Real inc, Real raan,
//                   Real aop, Real ma, Real mu,
//                   const Rvector3 &p, const Rvector3 &q,
//                   Rvector3 &position, Rvector3 &velocity)
//------------------------------------------------------------------------------
void ToCartesian(Real sma, Real ecc, Real inc, Real raan,
                 Real aop, Real ma, Real mu,
                 const Rvector3 &p, const Rvector3 &q,
                 Rvector3 &position, Rvector3 &velocity)
{
    Radians eccAnomaly = MeanToEccentricAnomaly(ma, ecc);
 
    const Real sinEA = Sin(eccAnomaly);
    const Real cosEA = Cos(eccAnomaly);
 
    if (sma < 0.0 || ecc < 0.0 || ecc > 1.0)
    {
        throw EphemerisUtil::InvalidEllipticalElements();
    }
    //  Compute position vector and position magnitude
    int j;
    for (j = 0; j <= 2; j++)
    {
        position[j] = p[j] * sma * (cosEA - ecc) +
                 q[j] * sma * GmatMathUtil::Sqrt(1.0 - ecc*ecc) * sinEA;
    }
    Real r = position.GetMagnitude();
 
    for (j = 0; j <= 2; j++)
    {
        velocity[j] = GmatMathUtil::Sqrt(mu * sma) * (q[j] *
                 GmatMathUtil::Sqrt(1.0 - (ecc*ecc)) * cosEA - p[j] *
                 sinEA) / r;
    }
}

//------------------------------------------------------------------------------
//  <friend>
//  void ToCartesian(Real sma, Real ecc, Real inc, Real raan,
//                   Real aop, Real ma, Real mu,
//                   Rvector3 &position, Rvector3 &velocity)
//------------------------------------------------------------------------------
void ToCartesian(Real sma, Real ecc, Real inc, Real raan,
                 Real aop, Real ma, Real mu,
                 Rvector3 &position, Rvector3 &velocity)
{
    ToCartesian(sma, ecc, inc, raan, aop, ma, mu, P(inc, raan, aop),
                Q(inc, raan, aop), position, velocity);
}

//------------------------------------------------------------------------------
//  Rvector3 P(Radians inclination, Radians RAAscendingNode,
//            Radians argumentOfPeriapsis)
//------------------------------------------------------------------------------
Rvector3 P(Radians inclination, Radians RAAscendingNode,
                       Radians argumentOfPeriapsis)
{
    //  Local variables
    const Real cosArgPer = Cos(argumentOfPeriapsis);
    const Real cosI = Cos(inclination);
    const Real cosRA = Cos(RAAscendingNode);
    const Real sinArgPer = Sin(argumentOfPeriapsis);
    const Real sinI = Sin(inclination);
    const Real sinRA = Sin(RAAscendingNode);
 
    return Rvector3((cosArgPer * cosRA - sinArgPer * sinRA * cosI),
                   (cosArgPer * sinRA + sinArgPer * cosRA * cosI),
                   (sinArgPer * sinI));
 
}
 
//------------------------------------------------------------------------------
//  Rvector3 Q(Radians inclination, Radians RAAscendingNode,
//            Radians argumentOfPeriapsis)
//------------------------------------------------------------------------------
Rvector3 Q(Radians inclination, Radians RAAscendingNode,
                       Radians argumentOfPeriapsis)
{
    //  Local variables
    const Real cosArgPer = Cos(argumentOfPeriapsis);
    const Real cosI = Cos(inclination);
    const Real cosRA = Cos(RAAscendingNode);
    const Real sinArgPer = Sin(argumentOfPeriapsis);
    const Real sinI = Sin(inclination);
    const Real sinRA = Sin(RAAscendingNode);
 
    return Rvector3((-sinArgPer * cosRA - cosArgPer * sinRA * cosI),
                   (-sinArgPer * sinRA + cosArgPer * cosRA * cosI),
                   (cosArgPer * sinI) );
}
