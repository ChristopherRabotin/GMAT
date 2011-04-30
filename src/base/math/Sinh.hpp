//$Id$
//------------------------------------------------------------------------------
//                              Sinh
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
// Author: Allison Greene
// Created: 2006/06/27
//
/**
 * Declares Sinh class.
 */
//------------------------------------------------------------------------------
#ifndef Sinh_hpp
#define Sinh_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API Sinh : public MathFunction
{
public:
   Sinh(const std::string &nomme);
   virtual ~Sinh();
   Sinh(const Sinh &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Sinh_hpp
