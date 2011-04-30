//------------------------------------------------------------------------------
//                              Log10
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
// Created: 2006/04/14
//
/**
 * Declares Log10 class.
 */
//------------------------------------------------------------------------------
#ifndef Log10_hpp
#define Log10_hpp

#include "MathFunction.hpp"

class GMAT_API Log10 : public MathFunction
{
public:
   Log10(const std::string &nomme);
   virtual ~Log10();
   Log10(const Log10 &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Log10_hpp
