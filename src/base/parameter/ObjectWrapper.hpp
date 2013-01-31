//$Id$
//------------------------------------------------------------------------------
//                                  ObjectWrapper
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun/GSFC
// Created: 2007/07/24
//
/**
 * Declares ObjectWrapper class.
 */
//------------------------------------------------------------------------------
#ifndef ObjectWrapper_hpp
#define ObjectWrapper_hpp

#include "gmatdefs.hpp"
#include "ElementWrapper.hpp"

class GMAT_API ObjectWrapper : public ElementWrapper
{
public:

   ObjectWrapper();
   ObjectWrapper(const ObjectWrapper &copy);
   const ObjectWrapper& operator=(const ObjectWrapper &right);
   virtual ~ObjectWrapper();
   
   virtual ElementWrapper*     Clone() const;

   virtual std::string         ToString();
   virtual Gmat::ParameterType GetDataType() const;
   
   virtual GmatBase*           GetRefObject(const std::string &name = "");
   virtual bool                SetRefObject(GmatBase *obj);
   virtual bool                RenameObject(const std::string &oldName, 
                                            const std::string &newName);
   
   virtual Real                EvaluateReal() const;
   virtual bool                SetReal(const Real val);
   
   virtual GmatBase*           EvaluateObject() const;
   virtual bool                SetObject(GmatBase* obj);
   
protected:  

   // the object pointer
   GmatBase *theObject;
   
   virtual void         SetupWrapper();
};
#endif // ObjectWrapper_hpp
