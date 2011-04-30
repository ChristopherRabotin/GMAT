//$Id$
//------------------------------------------------------------------------------
//                            File: BurnParameters.hpp
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
// Author: Linda Jun
// Created: 2005/05/27
//
/**
 * Declares Burn related parameter classes.
 *   BurnElements
 */
//------------------------------------------------------------------------------
#ifndef BurnParameters_hpp
#define BurnParameters_hpp

#include "gmatdefs.hpp"
#include "BurnReal.hpp"


//==============================================================================
//                               ImpBurnElements
//==============================================================================
/**
 * Declares Burn related Parameter classes.
 *    ImpBurnElements
 */
//------------------------------------------------------------------------------

class GMAT_API ImpBurnElements : public BurnReal
{
public:

   ImpBurnElements(const std::string &type = "",
                   const std::string &name = "",
                   GmatBase *obj = NULL);
   ImpBurnElements(const ImpBurnElements &copy);
   ImpBurnElements& operator=(const ImpBurnElements &right);
   virtual ~ImpBurnElements();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mElementId;
};


#endif // BurnParameters_hpp
