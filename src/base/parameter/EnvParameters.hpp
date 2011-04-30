//$Id$
//------------------------------------------------------------------------------
//                             File: EnvParameters.hpp
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
// Created: 2004/12/10
//
/**
 * Declares Environment related parameter classes.
 *   AtmosDensity
 */
//------------------------------------------------------------------------------
#ifndef EnvParameters_hpp
#define EnvParameters_hpp

#include "EnvReal.hpp"

//==============================================================================
//                              AtmosDensity
//==============================================================================
/**
 * Declares atmospheric density parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AtmosDensity : public EnvReal
{
public:

   AtmosDensity(const std::string &name = "", GmatBase *obj = NULL);
   AtmosDensity(const AtmosDensity &copy);
   const AtmosDensity& operator=(const AtmosDensity &right);
   virtual ~AtmosDensity();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

#endif //EnvParameters_hpp
