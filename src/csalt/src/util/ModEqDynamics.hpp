#ifndef ModEqDynamics_hpp
#define ModEqDynamics_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

class CSALT_API ModEqDynamics
{
public:
   ModEqDynamics(Real centralBodyGravParam);
   ModEqDynamics(const ModEqDynamics &copy);
   ModEqDynamics& operator=(const ModEqDynamics &copy);
   virtual ~ModEqDynamics();

   virtual Rvector ComputeOrbitDerivatives(Rvector orbitElements, Rvector pertForces);

protected:
   /// Central body gravitational parameter (i.e. Mu)
   Real gravParam;
   /// Absolute integration tolerance
   Real absTol;
   /// Relative integration tolerance
   Real relTol;
};
#endif
