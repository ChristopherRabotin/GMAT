//$Header$
//------------------------------------------------------------------------------
//                                  ObjectWrapper
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
   
   virtual Gmat::ParameterType GetDataType() const;
   
   virtual bool         SetRefObject(GmatBase *obj);
   virtual bool         RenameObject(const std::string &oldName, 
                                     const std::string &newName);
   
   virtual Real         EvaluateReal() const;
   virtual bool         SetReal(const Real val);
   
   virtual GmatBase*    EvaluateObject() const;
   virtual bool         SetObject(const GmatBase* obj);
   
protected:  

   // the object pointer
   GmatBase *theObject;
   
   virtual void         SetupWrapper();
};
#endif // ObjectWrapper_hpp
