//$Header$
//------------------------------------------------------------------------------
//                              PropState
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/07/24
//
/**
 * Defines the state used in propagation. 
 */
//------------------------------------------------------------------------------


#ifndef PropState_hpp
#define PropState_hpp

#include "gmatdefs.hpp"

class GMAT_API PropState
{
public:

   PropState(const Integer dim = 6);
   virtual ~PropState();
   PropState(const PropState& ps);
   PropState&        operator=(const PropState& ps);
 
   // Array manipulations
   Real&             operator[](const Integer el);
   Real              operator[](const Integer el) const;
   
   // Sizing manipulation
   void              SetSize(const Integer size);
   
   // Access methods
   Integer           GetSize() const;
   Real*             GetState();
   bool              SetState(const Real *data, const Integer size);
   Real              GetEpoch() const;
   Real              SetEpoch(const Real ep);
   
 
protected:
   /// Array used for the state data
   Real              *state;
   /// Size of the state vector
   Integer           dimension;
   /// Raw epoch data for the state
   Real              epoch;
};

#endif // PropState_hpp
