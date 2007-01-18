//$Header$
//------------------------------------------------------------------------------
//                                  ParameterWrapper
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
   ParameterWrapper(const std::string &desc = "");
   // copy constructor
   ParameterWrapper(const ParameterWrapper &pr);
   // operator = 
   const ParameterWrapper& operator=(const ParameterWrapper &pr);
   // destructor
   virtual ~ParameterWrapper();
   
   virtual bool            SetParameter(Parameter *toParam);
   
   virtual Real            EvaluateReal() const;
   
   virtual bool            SetReal(const Real toValue);
   
   
protected:  

   // the Real value
   Parameter *param;
   
};
#endif // ParameterWrapper_hpp
