//$Header$ 
//------------------------------------------------------------------------------
//                                 StateConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2004/03/22
//
/**
 * Definition of the StateConverter class
 */
//------------------------------------------------------------------------------

#ifndef StateConverter_hpp
#define StateConverter_hpp

// #include "Converter.hpp"
#include "SolarSystem.hpp" 
#include "CelestialBody.hpp"
#include "A1Date.hpp"
#include "A1Mjd.hpp"
#include "UtcDate.hpp"
// #include "Cartesian.hpp"
// #include "Keplerian.hpp"
#include "CoordUtil.hpp" 
#include "SphericalAZFPA.hpp"
 #include "SphericalRADEC.hpp"
#include "Rvector6.hpp"

class GMAT_API StateConverter 
{
public:
   // Default constructor
   StateConverter();
   StateConverter(const std::string &newType);
   StateConverter(const std::string &newType, const Real newMu);
//   StateConverter(SolarSystem *solarSystem, const std::string &type);

   // Copy constructor
   StateConverter(const StateConverter &state);
   // Assignment operator
   StateConverter& operator=(const StateConverter &state);

   // Destructor
   virtual ~StateConverter();

   // public method 
   Real     GetMu() const;
   bool     SetMu(SolarSystem *solarSystem, const std::string &body);

   //loj: 10/21/04 added
   Rvector6 Convert(const Rvector6 &state, const std::string &fromElementType,
                    const std::string &toElementType);

   Rvector6 Convert(const Real *state, const std::string &fromElementType,
                    const std::string &toElementType);

   Rvector6 Convert(const Real *state, const std::string &toElementType);
   
   // Constant variable
   static const Real DEFAULT_MU = 0.3986004415e+06;  // km^3/s^2
 
protected:
   // Declare protetced method data 

private:
   std::string  type;
   Real         mu;

};

#endif // StateConverter_hpp
