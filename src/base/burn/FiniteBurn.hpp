//$Header$
//------------------------------------------------------------------------------
//                              FiniteBurn
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
 * Defines the FiniteBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#ifndef FiniteBurn_hpp
#define FiniteBurn_hpp

#include "Burn.hpp"


class GMAT_API FiniteBurn : public Burn
{
public:
   FiniteBurn(std::string nomme = "");
   virtual ~FiniteBurn();
   FiniteBurn(const FiniteBurn& fb);
   FiniteBurn&             operator=(const FiniteBurn& fb);
   
   virtual bool            Fire(Real *burnData = NULL);
   
   GmatBase*               Clone() const;
   
protected:
   
};

#endif // FiniteBurn_hpp
