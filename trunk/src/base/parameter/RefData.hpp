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
   RefData(const RefData &copy);
   RefData& operator= (const RefData &right);
   virtual ~RefData();

   Integer GetNumRefObjects() const;
   
   std::string GetRefObjectName(const Gmat::ObjectType type) const;
   const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   
   bool SetRefObjectName(const Gmat::ObjectType type,
                         const std::string &name);
   GmatBase* GetRefObject(const Gmat::ObjectType type,
                          const std::string &name = "");
   bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                     const std::string &name = "");

   bool RenameRefObject(const Gmat::ObjectType type,
                        const std::string &oldName,
                        const std::string &newName);
   
   virtual bool ValidateRefObjects(GmatBase *param) = 0;
   virtual const std::string* GetValidObjectList() const;

protected:

   static const int MAX_OBJ_COUNT = 5;
   
   struct RefObjType
   {
      Gmat::ObjectType objType;
      std::string objName;
      GmatBase *obj;
   };

   RefObjType mRefObjList[MAX_OBJ_COUNT];
   
   StringArray mObjectTypeNames;
   StringArray mAllRefObjectNames;
   Integer mNumRefObjects;

   bool AddRefObject(const Gmat::ObjectType type,
                     const std::string &name, GmatBase *obj = NULL,
                     bool replaceName = false);
   
   bool SetRefObjectWithNewName(GmatBase *obj, const Gmat::ObjectType type,
                                const std::string &name);
   
   bool HasObjectType(const std::string &type) const;
   GmatBase* FindFirstObject(const std::string &type) const;
   GmatBase* FindFirstObject(const Gmat::ObjectType type) const;
   std::string FindFirstObjectName(const Gmat::ObjectType type) const;
   
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type) = 0;
};
#endif // RefData_hpp

