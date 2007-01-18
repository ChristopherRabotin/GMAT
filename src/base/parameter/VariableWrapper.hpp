//$Header$
//------------------------------------------------------------------------------
//                                  VariableWrapper
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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
   VariableWrapper(const std::string &desc = "");
   // copy constructor
   VariableWrapper(const VariableWrapper &vr);
   // operator = 
   const VariableWrapper& operator=(const VariableWrapper &vr);
   // destructor
   virtual ~VariableWrapper();
   
   virtual bool            SetVariable(Variable *toVar);
   
   virtual Real            EvaluateReal() const;
   
   virtual bool            SetReal(const Real toValue);
   
   
protected:  

   // the Real value
   Variable *var;
   
};
#endif // VariableWrapper_hpp
