//$Header$
//------------------------------------------------------------------------------
//                                  RefData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/09
//
/**
 * Declares base class of reference data.
 */
//------------------------------------------------------------------------------
#ifndef RefData_hpp
#define RefData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include <map>

class GMAT_API RefData
{
public:

   RefData();
   RefData(const RefData &data);
   RefData& operator= (const RefData& right);
   virtual ~RefData();

   Integer GetNumRefObjects() const;
   virtual GmatBase* GetRefObject(const std::string &objType);
    
   virtual bool SetRefObject(Gmat::ObjectType objType,
                             const std::string &objName,
                             GmatBase *obj);
    
   bool AddRefObject(GmatBase *obj);

   virtual bool ValidateRefObjects(GmatBase *param) = 0;
   virtual const std::string* GetValidObjectList() const;

protected:
   bool HasObjectType(const std::string &objType) const;
   GmatBase* FindFirstObject(const std::string &objType) const; //loj: 4/27/04 added
     
   virtual void InitializeRefObjects(); //loj: 3/31/04 added
   virtual bool IsValidObject(GmatBase *obj) = 0;
   std::map<std::string, GmatBase*> *mObjTypeObjMap;
   StringArray mObjectTypeNames;
   Integer mNumRefObjects;
};
#endif // RefData_hpp

