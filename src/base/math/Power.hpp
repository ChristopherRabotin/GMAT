//------------------------------------------------------------------------------
//                              Power
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
// Author: LaMont Ruley
// Created: 2006/04/20
//
/**
 * Declares Power class.
 */
//------------------------------------------------------------------------------
#ifndef Power_hpp
#define Power_hpp

#include "MathFunction.hpp"

class GMAT_API Power : public MathFunction
{
public:
   Power(const std::string &nomme);
   virtual ~Power();
   Power(const Power &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Power_hpp
