#include "ModEqDynamics.hpp"

//------------------------------------------------------------------------------
// ModEqDynamics()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
 //------------------------------------------------------------------------------
ModEqDynamics::ModEqDynamics(Real centralBodyGravParam) :
   gravParam (centralBodyGravParam),
   absTol    (1.0e-8),
   relTol    (1.0e-8)
{

}

//------------------------------------------------------------------------------
// ModEqDynamics(const ModEqDynamics &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
 //------------------------------------------------------------------------------
ModEqDynamics::ModEqDynamics(const ModEqDynamics &copy) :
   gravParam (copy.gravParam),
   absTol    (copy.absTol),
   relTol    (copy.relTol)
{

}

//------------------------------------------------------------------------------
// ModEqDynamics& operator=(const ModEqDynamics &copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
 //------------------------------------------------------------------------------
ModEqDynamics& ModEqDynamics::operator=(const ModEqDynamics &copy)
{
   if (&copy == this)
      return *this;

   gravParam = copy.gravParam;
   absTol    = copy.absTol;
   relTol    = copy.relTol;

   return *this;
}

//------------------------------------------------------------------------------
// ~ModEqDynamics()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
 //------------------------------------------------------------------------------
ModEqDynamics::~ModEqDynamics()
{

}

Rvector ModEqDynamics::ComputeOrbitDerivatives(Rvector orbitElements, Rvector pertForces)
{
   // Extract elements
   Real p = orbitElements(0);
   Real f = orbitElements(1);
   Real g = orbitElements(2);
   Real h = orbitElements(3);
   Real k = orbitElements(4);
   Real L = orbitElements(5);

   // Compute repeated terms.  See Betts near Eq. 6.37
   Real sqrtPbyMu = GmatMathUtil::Sqrt(p / gravParam);
   Real sinL = GmatMathUtil::Sin(L);
   Real cosL = GmatMathUtil::Cos(L);
   Real hsinL = h * sinL;
   Real kcosL = k * cosL;
   Real q = 1 + f * cosL + g * sinL;
   Real chi = GmatMathUtil::Sqrt(h * h + k * k);
   Real sSquared = 1 + chi * chi;
   Real fac1 = sqrtPbyMu * sSquared / 2 / q;

   // Compute the A matrix.  Eq 6.35 of Betts
   Rmatrix A(6, 3);
   A(0, 1) = 2.0 * p / q * sqrtPbyMu;
   A(1, 0) = sqrtPbyMu * sinL;
   A(1, 1) = sqrtPbyMu / q * ((q + 1.0) * cosL + f);
   A(2, 1) = sqrtPbyMu / q * ((q + 1.0) * sinL + g);
   A(2, 0) = -sqrtPbyMu * cosL;
   A(1, 2) = -sqrtPbyMu * g / q * (hsinL - kcosL);
   A(2, 2) = sqrtPbyMu * f / q * (hsinL - kcosL);
   A(3, 2) = fac1 * cosL;
   A(4, 2) = fac1 * sinL;
   A(5, 2) = sqrtPbyMu / q * (hsinL - kcosL);

   Rvector b(6);
   b(5) = GmatMathUtil::Sqrt(gravParam * p) * (q * q) / (p * p);

   // Compute the derivatives.  Eq. 6.31 of Betts
   Rvector odeRate = A * pertForces + b;
   return odeRate;
}
