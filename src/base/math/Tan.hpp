//------------------------------------------------------------------------------
//                              Tan
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
// Created: 2006/04/10
//
/**
 * Declares Tan class.
 */
//------------------------------------------------------------------------------
#ifndef Tan_hpp
#define Tan_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API Tan : public MathFunction
{
public:
   Tan(const std::string &nomme);
   virtual ~Tan();
   Tan(const Tan &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Tan_hpp
