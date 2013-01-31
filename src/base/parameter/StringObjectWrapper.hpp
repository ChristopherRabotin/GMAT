//$Id$
//------------------------------------------------------------------------------
//                                  StringObjectWrapper
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
// Created: 2007.04.04
//
/**
 * Definition of the StringObjectWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef StringObjectWrapper_hpp
#define StringObjectWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"
#include "StringVar.hpp"

class GMAT_API StringObjectWrapper : public ElementWrapper
{
public:

   // default constructor
   StringObjectWrapper();
   // copy constructor
   StringObjectWrapper(const StringObjectWrapper &sow);
   // operator = 
   const StringObjectWrapper& operator=(const StringObjectWrapper &sow);
   // destructor
   virtual ~StringObjectWrapper();
   
   virtual ElementWrapper*     Clone() const;


   virtual Gmat::ParameterType GetDataType() const;
   
   virtual const StringArray& GetRefObjectNames();
   virtual bool               SetRefObject(GmatBase *obj);
   virtual Real               EvaluateReal() const;
   virtual bool               SetReal(const Real toValue);
   // need to override this method, to handle the arrayName, rowName,
   // and columnName data members
   virtual bool               RenameObject(const std::string &oldName, 
                                           const std::string &newName);
   
   virtual std::string     EvaluateString() const;
   virtual bool            SetString(const std::string &toValue); 
   
protected:  

   /// pointer to the String object
   StringVar       *stringVar;
   
   /// name of the array
   std::string     stringName;
   
   
   virtual void            SetupWrapper(); 
};
#endif // StringObjectWrapper_hpp
