//$Header$
//------------------------------------------------------------------------------
//                           ModKeplerian
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Arthor:  Joey Gurganus 
// Created: 2004/01/08 
//
/**
 * Definition for the Modifed Keplerian class including raduis of periapsis, 
 * raduis of apoapsis, inclination, right ascension of the ascending node, 
 * argument of periapsis, and anomaly.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ModKeplerian_hpp
#define ModKeplerian_hpp

#include <iostream>
#include <sstream>
#include "gmatdefs.hpp"
#include "Rvector6.hpp"
#include "RealTypes.hpp"
#include "RealUtilities.hpp"
#include "CoordUtil.hpp"
#include "UtilityException.hpp"

class ModKeplerian
{
public:
   ModKeplerian();
   ModKeplerian(const Rvector6& state);
   ModKeplerian(const Real rp, const Real ra, const Real inc, 
                     const Real mRAAN, const Real mAOP, const Real anom);
   ModKeplerian(const ModKeplerian &modKeplerian);
   ModKeplerian& operator=(const ModKeplerian &m);
   virtual ~ModKeplerian();

   //  Friend functions
   friend std::ostream& operator<<(std::ostream& output, ModKeplerian& m);
   friend std::istream& operator>>(std::istream& input, ModKeplerian& m);

   friend Rvector6 KeplerianToModKeplerian(const Rvector6& keplerian);
   friend Rvector6 ModKeplerianToKeplerian(const Rvector6& modKeplerian);

   // public methods
   Rvector6 GetState();
   void SetState(const Rvector6& state);

   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   std::string* ToValueStrings();

protected:

private:
   Real     radiusOfPeriapsis;      
   Real     radiusOfApoapsis;      
   Real     inclination;      
   Real     raan;              // Right ascension of the ascending node
   Real     aop;               // Argument of Periapsis 
   Real     anomaly;           // True Anomaly as default; otherwise 
    
   static const Integer NUM_DATA = 6;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];

};
#endif // ModKeplerian_hpp
