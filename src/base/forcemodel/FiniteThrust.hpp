//$Header$
//------------------------------------------------------------------------------
//                              FiniteThrust
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Defines the FiniteThrust class used to model the acceleration during a finite 
 * burn. 
 */
//------------------------------------------------------------------------------


#ifndef FiniteThrust_hpp
#define FiniteThrust_hpp

#include "PhysicalModel.hpp"
#include "FiniteBurn.hpp"

class GMAT_API FiniteThrust : public PhysicalModel
{
public:
   FiniteThrust(const std::string &name);
   virtual ~FiniteThrust();
   FiniteThrust(const FiniteThrust& ft);
   FiniteThrust&                 operator=(const FiniteThrust& ft);

   virtual bool                  GetDerivatives(Real * state, Real dt, 
                                                Integer order);
protected:
   std::vector <FiniteBurn *>    burns;
};

#endif // FiniteThrust_hpp
