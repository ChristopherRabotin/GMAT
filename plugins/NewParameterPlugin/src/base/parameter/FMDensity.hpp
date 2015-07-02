//$Id$
//------------------------------------------------------------------------------
//                             FMDensity
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/29
//
/**
 * Declares force model based density parameter class.
 */
//------------------------------------------------------------------------------
#ifndef FMDensity_hpp
#define FMDensity_hpp

#include "OdeReal.hpp"
#include "OdeData.hpp"

//==============================================================================
//                              FMDensity
//==============================================================================
/**
 * Declares atmospheric density parameter class.
 */
//------------------------------------------------------------------------------

class NEW_PARAMETER_API FMDensity : public OdeReal
{
public:

   FMDensity(const std::string &name = "", GmatBase *obj = NULL);
   FMDensity(const FMDensity &copy);
   const FMDensity& operator=(const FMDensity &right);
   virtual ~FMDensity();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

#endif //FMDensity_hpp
