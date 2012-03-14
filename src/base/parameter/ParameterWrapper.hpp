//$Id$
//------------------------------------------------------------------------------
//                                  ParameterWrapper
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
 * Definition of the ParameterWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef ParameterWrapper_hpp
#define ParameterWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"
#include "Parameter.hpp"

class GMAT_API ParameterWrapper : public ElementWrapper
{
public:

   // default constructor
   ParameterWrapper();
   // copy constructor
   ParameterWrapper(const ParameterWrapper &pw);
   // operator = 
   const ParameterWrapper& operator=(const ParameterWrapper &pw);
   // destructor
   virtual ~ParameterWrapper();
   
   virtual ElementWrapper*     Clone() const;
   virtual Gmat::ParameterType GetDataType() const;
   
   virtual GmatBase*           GetRefObject(const std::string &name = "");
   virtual bool                SetRefObject(GmatBase *obj);
   virtual bool                RenameObject(const std::string &oldName, 
                                            const std::string &newName);
   
   virtual Real                EvaluateReal() const;
   virtual bool                SetReal(const Real toValue);
   
   virtual std::string         EvaluateString() const;
   virtual bool                SetString(const std::string &toValue);
   
   virtual const Rmatrix&      EvaluateArray() const;
   virtual bool                SetArray(const Rmatrix &toValue);
   
   virtual GmatBase*           EvaluateObject() const;
   virtual bool                SetObject(const GmatBase* obj);
   
protected:  
   
   // pointer to the Parameter object
   Parameter *param;
   
   virtual void                SetupWrapper(); 
};
#endif // ParameterWrapper_hpp
