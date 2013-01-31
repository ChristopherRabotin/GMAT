//$Id$
//------------------------------------------------------------------------------
//                                  StringWrapper
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
 * Declares StringWrapper class.
 */
//------------------------------------------------------------------------------
#ifndef StringWrapper_hpp
#define StringWrapper_hpp

#include "gmatdefs.hpp"
#include "ElementWrapper.hpp"

class GMAT_API StringWrapper : public ElementWrapper
{
public:
   
   StringWrapper();
   StringWrapper(const StringWrapper &copy);
   const StringWrapper& operator=(const StringWrapper &right);
   virtual ~StringWrapper();
   
   virtual ElementWrapper*     Clone() const;

   virtual Gmat::ParameterType GetDataType() const;
   
   virtual Real         EvaluateReal() const;
   virtual bool         SetReal(const Real val);
   
   virtual std::string  EvaluateString() const;
   virtual bool         SetString(const std::string &val);
   
protected:  
   
   // the bool value
   std::string value;
   
   virtual void         SetupWrapper();
};
#endif // StringWrapper_hpp
