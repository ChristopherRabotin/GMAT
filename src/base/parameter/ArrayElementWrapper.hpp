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
   ArrayElementWrapper();
   // copy constructor
   ArrayElementWrapper(const ArrayElementWrapper &aew);
   // operator = 
   const ArrayElementWrapper& operator=(const ArrayElementWrapper &aew);
   // destructor
   virtual ~ArrayElementWrapper();
   
   virtual const StringArray& GetRefObjectNames();
   virtual bool               SetRefObject(GmatBase *obj);
   virtual Real               EvaluateReal() const;
   virtual bool               SetReal(const Real toValue);
   
   /// additioanl methods needed to handle the row and column elements
   virtual std::string        GetRowName();
   virtual std::string        GetColumnName();
   
   virtual bool               SetRow(ElementWrapper* toWrapper);
   virtual bool               SetColumn(ElementWrapper* toWrapper);
   
protected:  

   /// pointer to the Array object
   Array          *array;
   /// pointers to the wrappers for the row and column
   ElementWrapper *row;
   ElementWrapper *column;   
   
   /// name of the array
   std::string     arrayName;
   
   // names (description strings) for those row and column wrappers
   std::string     rowName;
   std::string     columnName;
   
   virtual void            SetupWrapper(); 
};
#endif // ArrayElementWrapper_hpp
