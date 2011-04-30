//$Id$
//------------------------------------------------------------------------------
//                                  NumberWrapper
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2007.01.18
//
/**
 * Definition of the NumberWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef NumberWrapper_hpp
#define NumberWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"

class GMAT_API NumberWrapper : public ElementWrapper
{
public:

   // default constructor
   NumberWrapper();
   // copy constructor
   NumberWrapper(const NumberWrapper &nr);
   // operator = 
   const NumberWrapper& operator=(const NumberWrapper &nr);
   // destructor
   virtual ~NumberWrapper();
   
   virtual ElementWrapper*    Clone() const;
   virtual Gmat::ParameterType GetDataType() const;
   
   virtual Real            EvaluateReal() const;  
   virtual bool            SetReal(const Real toValue);
   
   
protected:  

   // the Real value
   Real value;
   
   virtual void            SetupWrapper();
};
#endif // NumberWrapper_hpp
