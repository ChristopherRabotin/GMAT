//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the ImpulsiveBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#ifndef ImpulsiveBurn_hpp
#define ImpulsiveBurn_hpp


#include "Burn.hpp"


class GMAT_API ImpulsiveBurn : public Burn
{
public:
   ImpulsiveBurn(std::string nomme = "");
   ImpulsiveBurn(const ImpulsiveBurn &copy);
	virtual ~ImpulsiveBurn(void);
 
   virtual bool            Fire(Real *burnData = NULL);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;
};

#endif // ImpulsiveBurn_hpp
