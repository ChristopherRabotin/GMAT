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
 * Declares ephemeris exceptions and constants and conversions between
 * Cartesian and Keplerian elements.
 */
//------------------------------------------------------------------------------
#ifndef EphemerisUtil_hpp
#define EphemerisUtil_hpp

#include "gmatdefs.hpp"
#include "Rvector3.hpp"
#include "Cartesian.hpp"
#include "Keplerian.hpp"
#include "BaseException.hpp"

class EphemerisUtil
{
public:
    static const Real KEPLER_TOLERANCE = 1.0E-7;
    static const Integer MAX_ITERATIONS = 75;

    // Exceptions for Ephemeris types
    class InvalidEllipticalElements : public BaseException
          {public : InvalidEllipticalElements(const std::string &message =
          "Ephemeris error : Elliptical elements are not valid.")
          : BaseException(message) {};  };
    class ToleranceNotMet : public BaseException  
          {public : ToleranceNotMet(const std::string &message =
          "Ephemeris error : Kepler tolerance not met.")
          : BaseException(message) {};  };

    // conversion functions
    friend Keplerian ToKeplerian(const Rvector3 &position,
                                 const Rvector3 &velocity, Real mu);
    friend void ToKeplerian(const Rvector3 &position, const Rvector3 &velocity,
                            Real mu, Real &sma, Real &ecc, Real &inc, 
                            Real &raan, Real &aop, Real &ma);

    friend void ToCartesian(Real a, Real e, Real i, Real ra,
                            Real arg, Real ma, Real mu,
                            const Rvector3 &p, const Rvector3 &q,
                            Rvector3 &position, Rvector3 &velocity);

    friend void ToCartesian(Real a, Real e, Real i, Real ra,
                            Real arg, Real ma, Real mu,
                            Rvector3 &position, Rvector3 &velocity);

    friend Rvector3 P(Real inclination,
                      Real RAAscendingNode,
                      Real argumentOfPeriapsis);
 
    friend Rvector3 Q(Real inclination,
                      Real RAAscendingNode,
                      Real argumentOfPeriapsis);
};

#endif // EphemerisUtil_hpp
