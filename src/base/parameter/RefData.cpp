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
 * Implements base class of reference data.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "RefData.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_REFDATA_OBJECT 1
//#define DEBUG_RENAME 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RefData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
RefData::RefData()
{
   for (int i=0; i<MAX_OBJ_COUNT; i++)
   {
      mRefObjList[i].objType = Gmat::UNKNOWN_OBJECT;
      mRefObjList[i].objName = "";
      mRefObjList[i].obj = NULL;
   }

   mNumRefObjects = 0;
}

//------------------------------------------------------------------------------
// RefData(const RefData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the RefData object being copied.
 */
//------------------------------------------------------------------------------
RefData::RefData(const RefData &copy)
{
   for (int i=0; i<MAX_OBJ_COUNT; i++)
   {
      mRefObjList[i].objType = copy.mRefObjList[i].objType;
      mRefObjList[i].objName = copy.mRefObjList[i].objName;
      mRefObjList[i].obj = NULL;
   }
   
   mNumRefObjects = copy.mNumRefObjects;
}

//------------------------------------------------------------------------------
// RefData& operator= (const RefData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
RefData& RefData::operator= (const RefData& right)
{
   for (int i=0; i<MAX_OBJ_COUNT; i++)
   {
      mRefObjList[i].objType = right.mRefObjList[i].objType;
      mRefObjList[i].objName = right.mRefObjList[i].objName;
      mRefObjList[i].obj = NULL;
   }
   
   mNumRefObjects = right.mNumRefObjects;
   return *this;
}

//------------------------------------------------------------------------------
// ~RefData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RefData::~RefData()
{
}

//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer RefData::GetNumRefObjects() const
{
   return mNumRefObjects;
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
std::string RefData::GetRefObjectName(const Gmat::ObjectType type) const
{
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         //loj:
         //Notes: will return first object name.
         //       How do we handle multiple objects with same type?
#if DEBUG_REFDATA_OBJECT
         MessageInterface::ShowMessage
            ("RefData::GetRefObjectName() type=%d returning:%s\n", type,
             mRefObjList[i].objName.c_str());
#endif
         return mRefObjList[i].objName; 
      }
   }

#if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::GetRefObjectName() type=%d returning:INVALID_OBJECT_TYPE\n", type);
#endif
   return "INVALID_OBJECT_TYPE";
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Adds type and name to reference object list.
 *
 * @param <type> reference object type
 * @param <name> reference object name
 *
 * @return true if type and name has successfully added to the list
 *
 */
//------------------------------------------------------------------------------
bool RefData::SetRefObjectName(Gmat::ObjectType type,
                               const std::string &name)
{
#if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObjectName() type=%d, name=%s\n", type, name.c_str());
#endif
   return AddRefObject(type, name);
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type,
//                        const std::string &name);
//------------------------------------------------------------------------------
GmatBase* RefData::GetRefObject(const Gmat::ObjectType type,
                                const std::string &name)
{
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         if (mRefObjList[i].objName == name)
            return mRefObjList[i].obj; 
      }
   }

   return NULL;
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name = "");
//------------------------------------------------------------------------------
/**
 * Sets object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                           const std::string &name)
{
#if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObject() numRefObjects=%d, type=%d, name=%s\n",
       mNumRefObjects, type, name.c_str());
#endif
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         if (mRefObjList[i].objName == name)
         {
            mRefObjList[i].obj = obj;
#if DEBUG_REFDATA_OBJECT
            MessageInterface::ShowMessage
               ("RefData::SetRefObject() obj set to %s\n",
                obj->GetName().c_str());
#endif
            return true;
         }
      }
   }

#if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObject() Cannot find type=%d, name=%s\n", type, name.c_str());
#endif
   return false;
}

//loj: 11/16/04 added
//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool RefData::RenameRefObject(const Gmat::ObjectType type,
                              const std::string &oldName,
                              const std::string &newName)
{
#if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("RefData::RenameRefObject() type=%d, oldName=%s, newName=%s\n",
       type, oldName.c_str(), newName.c_str());
#endif

   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         if (mRefObjList[i].objName == oldName)
         {
            mRefObjList[i].objName = newName;
#if DEBUG_RENAME
            MessageInterface::ShowMessage
               ("RefData::RenameRefObject() renamed to:%s\n",
                mRefObjList[i].objName.c_str());
#endif
            return true;
         }
      }
   }

   return false;
}

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* RefData::GetValidObjectList() const
{
   return NULL;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool AddRefObject(const Gmat::ObjectType type,
//                   const std::string &name, GmatBase *obj = NULL)
//------------------------------------------------------------------------------
/**
 * Adds object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::AddRefObject(const Gmat::ObjectType type,
                           const std::string &name, GmatBase *obj)
{
#if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::AddRefObject() type=%d, name=%s\n", type, name.c_str());
#endif
   if (IsValidObjectType(type))
   {
      if (mNumRefObjects < MAX_OBJ_COUNT)
      {
         mRefObjList[mNumRefObjects].objType = type;
         mRefObjList[mNumRefObjects].objName = name;
         mRefObjList[mNumRefObjects].obj = obj;
         mNumRefObjects++;
         return true;
      }
   }

   return false;
}

//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void RefData::InitializeRefObjects()
{
}

//------------------------------------------------------------------------------
// bool HasObjectType(const std::string &type) const
//------------------------------------------------------------------------------
/**
 * @return true if it has the given object type, false otherwise
 */
//------------------------------------------------------------------------------
bool RefData::HasObjectType(const std::string &type) const
{

   for (int i=0; i<mNumRefObjects; i++)
   {
      if (GmatBase::GetObjectTypeString(mRefObjList[i].objType) == type)
         return true;
   }

   return false;
}

//------------------------------------------------------------------------------
// GmatBase* FindFirstObject(const std::string &type) const
//------------------------------------------------------------------------------
/**
 * @return forst object found for given type name.
 */
//------------------------------------------------------------------------------
GmatBase* RefData::FindFirstObject(const std::string &type) const
{
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (GmatBase::GetObjectTypeString(mRefObjList[i].objType) == type)
         return mRefObjList[i].obj;
   }

   return NULL;
   
//     std::map<std::string, GmatBase*>::iterator pos;
   
//     pos = mObjTypeObjMap->find(type);
   
//     if (pos != mObjTypeObjMap->end())
//     {
//  #if DEBUG_REFDATA_OBJECT
//        MessageInterface::ShowMessage
//           ("RefData::FindFirstObject(): type=%s "
//            "objName=%s\n", (pos->second->GetTypeName()).c_str(),
//            (pos->second->GetName()).c_str());
//  #endif
//        return pos->second;
//     }
   
//     return NULL;
}

