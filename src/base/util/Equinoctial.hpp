#ifndef EQUINOCTIAL_HPP_
#define EQUINOCTIAL_HPP_

#include <iostream>
#include <sstream>
#include "gmatdefs.hpp"
#include "Rmatrix33.hpp"
#include "Rvector6.hpp"
#include "RealTypes.hpp"
#include "RealUtilities.hpp"
#include "CoordUtil.hpp"
#include "UtilityException.hpp"

class Equinoctial {
public:
   Equinoctial(); 
   Equinoctial(const Rvector6& state);
   Equinoctial(const Real& ma, const Real& pEY, const Real& pEX, const Real& pNY, const Real& pNX, const Real& ml);
   Equinoctial(const Equinoctial &eq);
   Equinoctial& operator=(const Equinoctial &m);
   virtual ~Equinoctial();

   //  Friend functions
   friend std::ostream& operator<<(std::ostream& output, const Equinoctial& m);
   friend std::istream& operator>>(std::istream& input, Equinoctial& m);

   friend Rvector6 CartesianToEquinoctial(const Rvector6& cartesian, const Real& grav);
   friend Rvector6 EquinoctialToCartesian(const Rvector6& equinoctial, const Real& grav);

   // public methods
   Rvector6 GetState();
   void SetState(const Rvector6& state);

   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   std::string* ToValueStrings();

private:
   Real     semiMajor;
   Real     projEccY;
   Real     projEccX;
   Real     projNY;
   Real     projNX;
   Real     meanLongitude;
    
   static const Integer NUM_DATA = 6;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];
};

Rmatrix33 SetQ(Real, Real, Real);

#endif /*EQUINOCTIAL_HPP_*/
