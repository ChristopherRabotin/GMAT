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
// Keplerian ToKeplerian(const Rvector3& position, const Rvector3& velocity,
//                       Real mu)
//------------------------------------------------------------------------------
Keplerian ToKeplerian(const Rvector3& position, const Rvector3& velocity,
                      Real mu)
{
   Real a, e, i, ra, arg, ma;
   ToKeplerian(position, velocity, mu, a, e, i, ra, arg, ma);
   return Keplerian(a, e, i, ra, arg, ma);
}

//------------------------------------------------------------------------------
//  <friend>
//  void ToKeplerian(const Rvector3& position, const Rvector3& velocity,
//                   Real mu, Real& a, Real& e, Real& i,
//                   Real& ra, Real& arg, Real& ma)
//------------------------------------------------------------------------------
void ToKeplerian(const Rvector3& position, const Rvector3& velocity,
                 Real mu, Real& a, Real& e, Real& i,
                 Real& ra, Real& arg, Real& ma)
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
        a   = 0.0;
        e   = 0.0;
        i   = 0.0;
        ra  = 0.0;
        arg = 0.0;
        ma   = 0.0;
        return;
    }

    //  Compute semimajor Axis (A)
    a = (mu * r) / (2.0 * mu - r * vSquared);

    if (a <= 0.0)
    {
        throw EphemerisUtil::InvalidEllipticalElements();
    }

    //  Compute eccentricity;
    eSquared = 1.0 - p/a;

    if (eSquared < -GmatRealConst::REAL_EPSILON)
    {
        throw EphemerisUtil::InvalidEllipticalElements();
    }
    else if (eSquared < 0.0)
    {
        e = 0.0;
    }
    else
    {
        e = GmatMathUtil::Sqrt(eSquared);
    }

    if (e >= (1.0 + GmatRealConst::REAL_EPSILON))
    {
        throw EphemerisUtil::InvalidEllipticalElements();
    }
    else if (e > 1.0)
    {
        e = 1.0;
    }

    //  Compute inclination
    i = ATan(GmatMathUtil::Sqrt(h[0]*h[0] + h[1]*h[1]), h[2]);

    //  Compute the Right Ascension of the Ascending Node (RA)
    if (GmatMathUtil::IsEqual(i, 0.0))
    {
        ra = 0.0;
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
        ra = ATan(h[0], -h[1]);
        ra = Mod(ra, TWO_PI);
        argLat = ATan(position[2], Sin(i) * (position[0]*Cos(ra) + 
                 position[1]* Sin(ra)));
    }

    if (argLat < 0.0)
    {
        argLat += TWO_PI;
    }

    //  Make Right Ascension of Ascending Node positive
    if (ra < 0.0)
    {
        ra += TWO_PI;
    }

    //  Compute the argument of Periapsis (argPer)
    if (GmatMathUtil::IsEqual(e, 0.0))
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
    arg = argLat - trueAnomaly;

    //  Make argument of Periapsis positive
    if (arg < 0.0)
    {
        arg += TWO_PI;
    }

    //  Compute Mean Anomaly
    if (GmatMathUtil::IsEqual(e, 0.0))
    {
        ma = argLat;
    }
    else
    {
        //  Compute eccentric Anomaly (eccAnomaly)
        muA = mu * a;

        if (muA < 0.0)
        {
            a   = 0.0;
            e   = 0.0;
            i   = 0.0;
            ra  = 0.0;
            arg = 0.0;
            ma   = 0.0;
            return;
        }

        sqRootMuA   = GmatMathUtil::Sqrt(muA);
        eccAnomaly  = ATan((rDotV / sqRootMuA), (1.0 - (r/a)));
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
//  void ToCartesian(Real a, Real e, Real i, Real ra,
//                   Real arg, Real m, Real mu,
//                   const Rvector3 &p, const Rvector3 &q,
//                   Rvector3& position, Rvector3& velocity)
//------------------------------------------------------------------------------
void ToCartesian(Real a, Real e, Real i, Real ra,
                 Real arg, Real m, Real mu,
                 const Rvector3 &p, const Rvector3 &q,
                 Rvector3& position, Rvector3& velocity)
{
    Radians eccAnomaly = MeanToEccentricAnomaly(m, e);
 
    const Real sinEA = Sin(eccAnomaly);
    const Real cosEA = Cos(eccAnomaly);
 
    if (a < 0.0 || e < 0.0 || e > 1.0)
    {
        throw EphemerisUtil::InvalidEllipticalElements();
    }
    //  Compute position vector and position magnitude
    int j;
    for (j = 0; j <= 2; j++)
    {
        position[j] = p[j] * a * (cosEA - e) +
                 q[j] * a * GmatMathUtil::Sqrt(1.0 - e*e) * sinEA;
    }
    Real r = position.GetMagnitude();
 
    for (j = 0; j <= 2; j++)
    {
        velocity[j] = GmatMathUtil::Sqrt(mu * a) * (q[j] *
                 GmatMathUtil::Sqrt(1.0 - (e*e)) * cosEA - p[j] *
                 sinEA) / r;
    }
}

//------------------------------------------------------------------------------
//  <friend>
//  void ToCartesian(Real a, Real e, Real i, Real ra,
//                   Real arg, Real m, Real mu,
//                   Rvector3& position, Rvector3& velocity)
//------------------------------------------------------------------------------
void ToCartesian(Real a, Real e, Real i, Real ra,
                 Real arg, Real m, Real mu,
                 Rvector3& position, Rvector3& velocity)
{
    ToCartesian(a, e, i, ra, arg, m, mu, P(i, ra, arg),
                Q(i, ra, arg), position, velocity);
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
