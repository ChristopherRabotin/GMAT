//$Header$
//------------------------------------------------------------------------------
//                                  ObjectPropertyWrapper
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
 * Definition of the ObjectPropertyWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef ObjectPropertyWrapper_hpp
#define ObjectPropertyWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"

class GMAT_API ObjectPropertyWrapper : public ElementWrapper
{
public:

   // default constructor
   ObjectPropertyWrapper(const std::string &desc = "");
   // copy constructor
   ObjectPropertyWrapper(const ObjectPropertyWrapper &opw);
   // operator = 
   const ObjectPropertyWrapper& operator=(const ObjectPropertyWrapper &opw);
   // destructor
   virtual ~ObjectPropertyWrapper();
   
   virtual bool            SetObjectAndId(GmatBase *toObj, Integer andId);
   
   virtual Real            EvaluateReal() const;
   
   virtual bool            SetReal(const Real toValue);
   
   
protected:  

   // pointer to the base object
   GmatBase *object;
   /// parameter Id for the property of the object
   Integer  parameterId;
   
};
#endif // ObjectPropertyWrapper_hpp
