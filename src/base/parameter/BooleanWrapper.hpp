//$Id$
//------------------------------------------------------------------------------
//                                  BooleanWrapper
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun/GSFC
// Created: 2007/07/24
//
/**
 * Declares BooleanWrapper class.
 */
//------------------------------------------------------------------------------
#ifndef BooleanWrapper_hpp
#define BooleanWrapper_hpp

#include "gmatdefs.hpp"
#include "ElementWrapper.hpp"

class GMAT_API BooleanWrapper : public ElementWrapper
{
public:

   BooleanWrapper();
   BooleanWrapper(const BooleanWrapper &copy);
   const BooleanWrapper& operator=(const BooleanWrapper &right);
   virtual ~BooleanWrapper();
   
   virtual ElementWrapper*     Clone() const;

   virtual Gmat::ParameterType GetDataType() const;
   
   virtual Real         EvaluateReal() const;
   virtual bool         SetReal(const Real val);
   
   virtual bool         EvaluateBoolean() const;
   virtual bool         SetBoolean(const bool val);
   
protected:  

   // the bool value
   bool value;
   
   virtual void         SetupWrapper();
};
#endif // BooleanWrapper_hpp
