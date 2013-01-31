//$Id$
//------------------------------------------------------------------------------
//                          ObjectPropertyWrapper
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
   ObjectPropertyWrapper();
   // copy constructor
   ObjectPropertyWrapper(const ObjectPropertyWrapper &opw);
   // operator = 
   const ObjectPropertyWrapper& operator=(const ObjectPropertyWrapper &opw);
   // destructor
   virtual ~ObjectPropertyWrapper();
   
   virtual ElementWrapper*     Clone() const;

   virtual Gmat::ParameterType GetDataType() const;
   
   virtual const StringArray&  GetRefObjectNames();
   virtual bool                SetRefObjectName(const std::string &name,
                                            Integer index);
   virtual GmatBase*           GetRefObject(const std::string &name = "");
   virtual bool                SetRefObject(GmatBase *obj);
   virtual bool                RenameObject(const std::string &oldName, 
                                        const std::string &newName);
   
   virtual Real                EvaluateReal() const;
   virtual bool                SetReal(const Real toValue);
   virtual std::string         EvaluateString() const;
   virtual bool                SetString(const std::string &toValue); 
   virtual std::string         EvaluateOnOff() const;
   virtual bool                SetOnOff(const std::string &toValue);
   virtual bool                EvaluateBoolean() const;
   virtual bool                SetBoolean(const bool toValue); 
   virtual Integer             EvaluateInteger() const;
   virtual bool                SetInteger(const Integer toValue); 
   virtual bool                SetObject(GmatBase* obj);
   
   const Integer               GetPropertyId();

   virtual bool                TakeRequiredAction() const;

protected:  

   /// pointer to the base object
   GmatBase    *object;
   /// array of property ID names
   StringArray propIDNames;
   /// parameter Id for the property of the object
   Integer     propID;
   /// owned object name
   std::string ownedObjName;
   
   virtual void            SetupWrapper(); 
   
};
#endif // ObjectPropertyWrapper_hpp
