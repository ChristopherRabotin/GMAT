//$Id$
//------------------------------------------------------------------------------
//                                  RefData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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


/**
 * Moved out of the RefObjType class so that the Visual Studio exports could be implemented
 */
struct GMAT_API RefObjType
{
   Gmat::ObjectType objType;
   std::string objName;
   GmatBase *obj;

   // Constructor -- default values required for the DevStudio export issues 
   RefObjType(Gmat::ObjectType refType = Gmat::UNKNOWN_OBJECT, 
      const std::string &refName = "", GmatBase *ref = NULL)
      {
         objType = refType;
         objName = refName;
         obj     = ref;
      };

   RefObjType& operator= (const RefObjType& right)
      {
         if (this == &right)
            return *this;
         objType = right.objType;
         objName = right.objName;
         obj     = right.obj;
         return *this;
      };
};

#ifdef EXPORT_TEMPLATES

    // Instantiate STL template classes used in GMAT  
    // This does not create an object. It only forces the generation of all
    // of the members of the listed classes. It exports them from the DLL 
    // and imports them into the .exe file.

    // This fixes std::string:
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<RefObjType>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<RefObjType>;

#endif

class GMAT_API RefData
{
public:

   RefData(const std::string &name = "");
   RefData(const RefData &rd);
   RefData& operator= (const RefData &rd);
   virtual ~RefData();
   
   GmatBase* GetSpacecraft();
   
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

   //struct RefObjType
   //{
   //   Gmat::ObjectType objType;
   //   std::string objName;
   //   GmatBase *obj;
   //   RefObjType(Gmat::ObjectType refType, const std::string &refName, GmatBase *ref)
   //      {
   //         objType = refType;
   //         objName = refName;
   //         obj     = ref;
   //      };
   //   RefObjType& operator= (const RefObjType& right)
   //      {
   //         if (this == &right)
   //            return *this;
   //         objType = right.objType;
   //         objName = right.objName;
   //         obj     = right.obj;
   //         return *this;
   //      };
   //};
   
   std::string mName;
   std::vector<RefObjType> mRefObjList;
   
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

