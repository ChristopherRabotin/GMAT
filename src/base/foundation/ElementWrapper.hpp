//$Header$
//------------------------------------------------------------------------------
//                                  ElementWrapper
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2007.01.17
//
/**
 * Definition of the ElementWrapper class.  This is the base class for wrappers 
 * for various element types.
 *
 * @note    As of 2007.01.17, derived classes handle elements of various 
 * parameter types - therefore, the derived class code is located in the
 * parameter subsystem.
 *
 */
//------------------------------------------------------------------------------

#ifndef ElementWrapper_hpp
#define ElementWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"

class GMAT_API ElementWrapper
{
public:

   // default constructor
   ElementWrapper();
   // copy constructor
   ElementWrapper(const ElementWrapper &er);
   // operator = 
   const ElementWrapper& operator=(const ElementWrapper &er);
   // destructor
   virtual ~ElementWrapper();
   
   virtual void               SetDescription(const std::string &str);
   virtual std::string        GetDescription() const;
   Gmat::WrapperDataType      GetWrapperType() const;
   
   virtual const StringArray& GetRefObjectNames();
   virtual bool               SetRefObject(GmatBase *obj);
   virtual bool               RenameObject(const std::string &oldName, 
                                           const std::string &newName);
   
   
//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped object.
 *
 * @return Real value of the wrapped object.
 * 
 * @note  This is a pure virtual (abstract) method and must be implemented in
 * all leaf classes derived from this one.
 */
//---------------------------------------------------------------------------
   virtual Real            EvaluateReal() const = 0;
   
//---------------------------------------------------------------------------
//  bool SetReal(const Real value)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 * 
 * @note  This is a pure virtual (abstract) method and must be implemented in
 * all leaf classes derived from this one.
 */
//---------------------------------------------------------------------------
   virtual bool            SetReal(const Real toValue) = 0;
   
   virtual Rmatrix         EvaluateArray() const;
   virtual bool            SetArray(const Rmatrix &toValue); 
   virtual std::string     EvaluateString() const;
   virtual bool            SetString(const std::string &toValue); 
   
protected:  

   static const Real UNDEFINED_REAL;
   /// Description of the wrapper
   std::string           description;
   StringArray           refObjectNames;
   
   Gmat::WrapperDataType wrapperType;

//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method that parses the description string so that the reference
 * objects and related properties are identified.
 *
 * @return true if successful; false otherwise.
 * 
 * @note  This is a pure virtual (abstract) method and must be implemented in
 * all leaf classes derived from this one.
 */
//---------------------------------------------------------------------------
   virtual void            SetupWrapper() = 0;
   
   
};
#endif // ElementWrapper_hpp
