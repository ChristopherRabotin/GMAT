//$Header$
//------------------------------------------------------------------------------
//                                  ArrayElementWrapper
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
 * Definition of the ArrayElementWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef ArrayElementWrapper_hpp
#define ArrayElementWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"
#include "Array.hpp"

class GMAT_API ArrayElementWrapper : public ElementWrapper
{
public:

   // default constructor
   ArrayElementWrapper(const std::string &desc = "");
   // copy constructor
   ArrayElementWrapper(const ArrayElementWrapper &aew);
   // operator = 
   const ArrayElementWrapper& operator=(const ArrayElementWrapper &aew);
   // destructor
   virtual ~ArrayElementWrapper();
   
   virtual bool            SetArray(Array *toArray);
   
   virtual bool            SetRowWrapper(ElementWrapper* toRow);
   
   virtual bool            SetColumnWrapper(ElementWrapper* toColumn);
   
   virtual Real            EvaluateReal() const;
   
   virtual bool            SetReal(const Real toValue);
   
   
protected:  

   /// pointer to the Array object
   Array          *array;
   /// pointers to the wrappers for the row and column
   ElementWrapper *row;
   ElementWrapper *column;   
};
#endif // ArrayElementWrapper_hpp
