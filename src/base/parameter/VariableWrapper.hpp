//$Id$
//------------------------------------------------------------------------------
//                                  VariableWrapper
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
 * Definition of the VariableWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef VariableWrapper_hpp
#define VariableWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"
#include "Variable.hpp"

class GMAT_API VariableWrapper : public ElementWrapper
{
public:

   // default constructor
   VariableWrapper();
   // copy constructor
   VariableWrapper(const VariableWrapper &vw);
   // operator = 
   const VariableWrapper& operator=(const VariableWrapper &vw);
   // destructor
   virtual ~VariableWrapper();
   
   virtual ElementWrapper*     Clone() const;
   virtual Gmat::ParameterType GetDataType() const;
   virtual std::string         ToString();  
   
   virtual GmatBase*           GetRefObject(const std::string &name = "");
   virtual bool                SetRefObject(GmatBase *obj);
   virtual bool                RenameObject(const std::string &oldName, 
                                            const std::string &newName);
   
   virtual Real                EvaluateReal() const;
   virtual bool                SetReal(const Real toValue);
   
protected:  

   // pointer to the Variable object
   Variable      *var;
   
   virtual void  SetupWrapper();
};
#endif // VariableWrapper_hpp
