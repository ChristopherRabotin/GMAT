//$Id$
//------------------------------------------------------------------------------
//                                  ArrayElementWrapper
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
   
   virtual ElementWrapper*     Clone() const;

   virtual Gmat::ParameterType GetDataType() const;
   
   virtual const StringArray& GetRefObjectNames();
   virtual GmatBase*          GetRefObject(const std::string &name = "");
   virtual bool               SetRefObject(GmatBase *obj);
   virtual Real               EvaluateReal() const;
   virtual bool               SetReal(const Real toValue);
   // need to override this method, to handle the arrayName, rowName,
   // and columnName data members
   virtual bool               RenameObject(const std::string &oldName, 
                                           const std::string &newName);
   
   /// additioanl methods needed to handle the row and column elements
   virtual std::string        GetRowName();
   virtual std::string        GetColumnName();
   virtual ElementWrapper*    GetRowWrapper();
   virtual ElementWrapper*    GetColumnWrapper();
   
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
