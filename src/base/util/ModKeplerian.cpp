//$Id$
//------------------------------------------------------------------------------
//                           ModKeplerian
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Arthor:  Joey Gurganus 
// Created: 2005/02/08 
//
/**
 * Definition for the Modified Keplerian class including raduis of periapsis, 
 * raduis of apoapsis, inclination, right ascension of the ascending node, 
 * argument of periapsis, and anomaly.
 * 
 */
//------------------------------------------------------------------------------

#include "ModKeplerian.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MOD_KEPLERIAN 1
//#define DEBUG_MODKEP_TO_KEP

//---------------------------------
//  static data
//---------------------------------
const std::string ModKeplerian::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Radius Of Periapsis",
   "Radius Of Apoapsis",
   "Inclincation",
   "RA of Ascending Node",
   "Argument of Periapsis",
   "Anomaly"
};

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  ModKeplerian::ModKeplerian() 
//------------------------------------------------------------------------------
/**
 * Constructs base Modified Keplerian structures 
 */
ModKeplerian::ModKeplerian() :
   radiusOfPeriapsis  (0.0),
   radiusOfApoapsis   (0.0),
   inclination        (0.0),
   raan               (0.0),
   aop                (0.0),
   anomaly            (0.0)
{
}

//------------------------------------------------------------------------------
//  ModKeplerian::ModKeplerian(const Rvector6& state)
//------------------------------------------------------------------------------
ModKeplerian::ModKeplerian(const Rvector6& state) :
   radiusOfPeriapsis  (state[0]),
   radiusOfApoapsis   (state[1]),
   inclination        (state[2]),
   raan               (state[3]),
   aop                (state[4]),
   anomaly            (state[5])
{
}

//------------------------------------------------------------------------------
//   ModKeplerian(const Real rp, const Real ra, const Real inc, 
//                     const Real mRAAN, const Real mAOP, const Real anom);
//------------------------------------------------------------------------------
ModKeplerian::ModKeplerian(const Real rp, const Real ra, 
                                     const Real inc, const Real mRAAN, 
                                     const Real mAOP, const Real anom) :
   radiusOfPeriapsis  (rp),
   radiusOfApoapsis   (ra),
   inclination        (inc),
   raan               (mRAAN),
   aop                (mAOP),
   anomaly            (anom)
{ 
}

//------------------------------------------------------------------------------
//   ModKeplerian::ModKeplerian(const ModKeplerian &modKeplerian)
//------------------------------------------------------------------------------
ModKeplerian::ModKeplerian(const ModKeplerian &modKeplerian) :
   radiusOfPeriapsis  (modKeplerian.radiusOfPeriapsis),
   radiusOfApoapsis   (modKeplerian.radiusOfApoapsis),
   inclination        (modKeplerian.inclination),
   raan               (modKeplerian.raan),
   aop                (modKeplerian.aop),
   anomaly            (modKeplerian.anomaly)
{
}

//------------------------------------------------------------------------------
//  ModKeplerian& ModKeplerian::operator=(const ModKeplerian &m)
//------------------------------------------------------------------------------
ModKeplerian& ModKeplerian::operator=(const ModKeplerian &m)
{
   if (this != &m)
   {
      radiusOfPeriapsis = m.radiusOfPeriapsis;
      radiusOfApoapsis  = m.radiusOfApoapsis;
      inclination       = m.inclination;
      raan              = m.raan;
      aop               = m.aop;
      anomaly           = m.anomaly;
   }
   return *this;
}

//------------------------------------------------------------------------------
//  ModKeplerian::~ModKeplerian() 
//------------------------------------------------------------------------------
ModKeplerian::~ModKeplerian() 
{
}

//------------------------------------------------------------------------------
//     friend functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  friend std::ostream& operator<<(std::ostream& output, ModKeplerian &m)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, ModKeplerian &m)
{
    Rvector v(6, m.radiusOfPeriapsis, m.radiusOfApoapsis, m.inclination,
              m.raan, m.aop, m.anomaly);

    output << v << std::endl;

    return output;
}

//------------------------------------------------------------------------------
//  friend std::istream& operator>>(std::istream& input, ModKeplerian &m)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, ModKeplerian &m)
{
    input >> m.radiusOfPeriapsis >> m.radiusOfApoapsis
          >> m.inclination >> m.raan
          >> m.aop >> m.anomaly;

    return input;
}


//------------------------------------------------------------------------------
//  friend Rvector6 KeplerianToModKeplerian(const Rvector6& keplerian)
//------------------------------------------------------------------------------
Rvector6 KeplerianToModKeplerian(const Rvector6& keplerian)
{
   #if DEBUG_MOD_KEPLERIAN
   MessageInterface::ShowMessage
      ("KeplerianToModKeplerian() keplerian =\n   %s\n", keplerian.ToString().c_str());
   #endif
   
   Real a = keplerian[0];    // Semi-major axis
   Real e = keplerian[1];    // eccentricity
   
   // Check for invalid eccentricity then send the error message
   if (e < 0)
      throw UtilityException("ModKeplerian::KeplerianToModKeplerian: " 
                             "ECC must be greater than 0");
   
   // Check for inconsistent semi-major axis and  eccentricity
   // then send the error message
   if (a > 0 && e > 1)
      throw UtilityException("ModKeplerian::KeplerianToModKeplerian: " 
                             "If ECC > 1, SMA must be negative");
   
   // Check for  exactly parabolic orbit or infinite semi-major axis
   // then send the error message
   if ( a == 1 || a == std::numeric_limits<Real>::infinity() )
      throw UtilityException("ModKeplerian::KeplerianToModKeplerian: " 
                             "Parabolic orbits cannot be entered in Keplerian "
                             "or Modified Keplerian format");
   
   // Check for parabolic orbit to machine precision
   // then send the error message
   if ( GmatMathUtil::Abs(e - 1) < 2*GmatRealConstants::REAL_EPSILON)
   {
//      throw UtilityException("ModKeplerian::KeplerianToModKeplerian: "
//                             "Orbit is nearly parabolic and state conversion "
//                             "routine is near numerical singularity");
      std::string errmsg =
            "Error in conversion from Keplerian to ModKeplerian state: ";
      errmsg += "The state results in an orbit that is nearly parabolic.\n";
      throw UtilityException(errmsg);
   }
   // Check for a singular conic section
   if (GmatMathUtil::Abs(a*(1 - e) < .001))
   {
      throw UtilityException
         ("Error in conversion from Keplerian to ModKeplerian state: "
          "The state results in a singular conic section with radius of periapsis less than 1 m.\n");
   }
   
   // Convert into radius of periapsis and apoapsis
   Real radPer = a*(1.0 - e);
   Real radApo = a*(1.0 + e);
   
   
   #if DEBUG_MOD_KEPLERIAN
   Rvector6 modkepl = Rvector6(radPer, radApo, keplerian[2], keplerian[3], 
                               keplerian[4], keplerian[5]);
   MessageInterface::ShowMessage
      ("KeplerianToModKeplerian() returning\n   %s\n", modkepl.ToString().c_str());
   #endif
   
   // return new Modified Keplerian
   return Rvector6(radPer, radApo, keplerian[2], keplerian[3], 
                   keplerian[4], keplerian[5]);
}


//------------------------------------------------------------------------------
//  friend Rvector6 ModKeplerianToKeplerian(const Rvector6& modKeplerian)
//------------------------------------------------------------------------------
Rvector6 ModKeplerianToKeplerian(const Rvector6& modKeplerian)
{
   #ifdef DEBUG_MODKEP_TO_KEP
      MessageInterface::ShowMessage("Entering ModKepToKep, radPer = %12.10f, radApo = %12.10f\n",
            modKeplerian[0], modKeplerian[1]);
   #endif
   Real radPer = modKeplerian[0];     // Radius of Periapsis
   Real radApo = modKeplerian[1];     // Radius of Apoapsis 

   // Check validity
   if (radApo < radPer && radApo > 0)
      throw UtilityException("ModKeplerian::ModKeplerianToKeplerian: If RadApo < RadPer then RadApo must be negative.  "
                             "If setting Modified Keplerian State, set RadApo before RadPer to avoid this issue.");

   if (radPer <= 0)
      throw UtilityException("ModKeplerian::ModKeplerianToKeplerian: " 
                             "Radius of Periapsis must be greater than zero");

   if (radApo == 0)
      throw UtilityException("ModKeplerian::ModKeplerianToKeplerian: " 
                             "Radius of Apoapsis must not be zero");

   if (radPer == 0)
      throw UtilityException("ModKeplerian::ModKeplerianToKeplerian: " 
                             "Parabolic orbits are not currently supported."
                             "RadPer must be greater than zero");

   // Compute the division between them
   Real rpbyra = radPer/radApo;

   // compute the eccentricity and semi-major axis
   Real e = (1.0 - rpbyra)/(1.0 + rpbyra);
   Real a = radPer/(1.0 - e);

   // Return the classic Keplerian
   return Rvector6(a, e, modKeplerian[2], modKeplerian[3],
                   modKeplerian[4], modKeplerian[5]);
}

//------------------------------------------------------------------------------
//     public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Rvector6 ModKeplerian::GetState() 
//------------------------------------------------------------------------------
Rvector6 ModKeplerian::GetState()
{
   return Rvector6(radiusOfPeriapsis, radiusOfApoapsis, inclination,
                   raan, aop, anomaly);
}

//------------------------------------------------------------------------------
// void ModKeplerian::SetState(const Rvector6& state) 
//------------------------------------------------------------------------------
void ModKeplerian::SetState(const Rvector6& state) 
{
   radiusOfPeriapsis = state[0];
   radiusOfApoapsis  = state[1];
   inclination       = state[2];
   raan              = state[3];
   aop               = state[4];
   anomaly           = state[5];
}

//------------------------------------------------------------------------------
// Integer ModKeplerian::GetNumData() const
//------------------------------------------------------------------------------
Integer ModKeplerian::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* ModKeplerian::GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* ModKeplerian::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
//  std::string* ModKeplerian::ToValueStrings(void)
//------------------------------------------------------------------------------
std::string* ModKeplerian::ToValueStrings(void)
{
   std::stringstream ss("");

   ss << radiusOfPeriapsis;
   stringValues[0] = ss.str();
   
   ss.str("");
   ss << radiusOfApoapsis;
   stringValues[1] = ss.str();
   
   ss.str("");
   ss << inclination;
   stringValues[2] = ss.str();
   
   ss.str("");
   ss << raan;
   stringValues[3] = ss.str();

   ss.str("");
   ss << aop;
   stringValues[4] = ss.str();

   ss.str("");
   ss << anomaly;
   stringValues[5] = ss.str();
   
   return stringValues;
}
