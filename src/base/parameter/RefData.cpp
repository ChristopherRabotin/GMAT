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
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_REFDATA_OBJECT 1
//#define DEBUG_REFDATA_OBJECT_GET 1
//#define DEBUG_REFDATA_FIND 1
//#define DEBUG_REFDATA_ADD 1
//#define DEBUG_RENAME 1
//#define DEBUG_CLONE 1

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
   mObjectTypeNames = copy.mObjectTypeNames;
   mAllRefObjectNames = copy.mAllRefObjectNames;
   mNumRefObjects = copy.mNumRefObjects;

   #if DEBUG_CLONE
   MessageInterface::ShowMessage
      ("RefData::RefData(const RefData &copy) mNumRefObjects=%d\n",
       mNumRefObjects);
   #endif
   
   for (int i=0; i<MAX_OBJ_COUNT; i++)
   {
      mRefObjList[i].objType = copy.mRefObjList[i].objType;
      mRefObjList[i].objName = copy.mRefObjList[i].objName;
      mRefObjList[i].obj = copy.mRefObjList[i].obj;
      
      #if DEBUG_CLONE > 1
      MessageInterface::ShowMessage
         ("i=%d, type=%d, name=%s, obj=%p\n", i, mRefObjList[i].objType,
          mRefObjList[i].objName.c_str(), mRefObjList[i].obj);
      #endif
   }

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
      mRefObjList[i].obj = right.mRefObjList[i].obj;
   }
   
   mObjectTypeNames = right.mObjectTypeNames;
   mAllRefObjectNames = right.mAllRefObjectNames;
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
   //MessageInterface::ShowMessage("==> RefData::~RefData()\n");
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
      #if DEBUG_REFDATA_OBJECT > 1
      MessageInterface::ShowMessage
         ("RefData::GetRefObjectName() mRefObjList[i].objType=%d\n",
          mRefObjList[i].objType);
      #endif
      
      if (mRefObjList[i].objType == type)
      {
         //Notes: will return first object name.
         #if DEBUG_REFDATA_OBJECT > 1
         MessageInterface::ShowMessage
            ("RefData::GetRefObjectName() type=%d returning: %s\n", type,
             mRefObjList[i].objName.c_str());
         #endif
         
         return mRefObjList[i].objName; 
      }
   }
   
   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::GetRefObjectName() type=%d returning:INVALID_OBJECT_TYPE\n", type);
   #endif
   
   //return "RefData::GetRefObjectName(): INVALID_OBJECT_TYPE";
   throw ParameterException("RefData::GetRefObjectName(): INVALID_OBJECT_TYPE");
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves reference object name array for given type. It will return all
 * object names if type is Gmat::UNKNOWN_NAME.
 *
 * @param <type> object type
 * @return reference object name.
 */
//------------------------------------------------------------------------------
const StringArray& RefData::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   mAllRefObjectNames.clear();

   #if DEBUG_REFDATA_OBJECT_GET
   MessageInterface::ShowMessage
      ("RefData::GetRefObjectNameArray() type=%d\n", type);
   #endif
   
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      for (int i=0; i<mNumRefObjects; i++)
      {
         #if DEBUG_REFDATA_OBJECT_GET > 1
         MessageInterface::ShowMessage
            ("RefData::GetRefObjectNameArray() objType=%d name: %s\n",
             mRefObjList[i].objType, mRefObjList[i].objName.c_str());
         #endif
         
         mAllRefObjectNames.push_back(mRefObjList[i].objName);
      }
   }
   else
   {
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == type)
         {
            #if DEBUG_REFDATA_OBJECT_GET > 1
            MessageInterface::ShowMessage
               ("RefData::GetRefObjectNameArray() type=%d name: %s\n", type,
                mRefObjList[i].objName.c_str());
            #endif
            
            mAllRefObjectNames.push_back(mRefObjList[i].objName);
         }
      }
   }
   
   return mAllRefObjectNames;
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
   
   if (FindFirstObjectName(type) != "")
   {
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == type)
         {
            mRefObjList[i].objName = name;
            return true;
         }
      }
   }
   
   return AddRefObject(type, name);
   
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type,
//                        const std::string &name = "");
//------------------------------------------------------------------------------
GmatBase* RefData::GetRefObject(const Gmat::ObjectType type,
                                const std::string &name)
{
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         if (name == "") //if name is "", return first object
            return mRefObjList[i].obj;
         
         if (mRefObjList[i].objName == name)
         {
            return mRefObjList[i].obj;
         }
      }
   }
   
   return NULL;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name = "")
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
   bool status = false;
   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObject() numRefObjects=%d, type=%d, name=%s obj addr=%p\n",
       mNumRefObjects, type, name.c_str(), obj);
   #endif

   // Since Sandbox calls SetRefObject() with obj->GetType(), I need to
   // set type to SPACE_POINT if CELESTIAL_BODY

   Gmat::ObjectType actualType = type;
   if (type == Gmat::CELESTIAL_BODY)
      actualType = Gmat::SPACE_POINT;
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == actualType)
      {
         if (mRefObjList[i].objName == name)
         {
            mRefObjList[i].obj = obj;
            #if DEBUG_REFDATA_OBJECT > 1
            MessageInterface::ShowMessage
               ("RefData::SetRefObject() set %s to %d\n",
                mRefObjList[i].objName.c_str(), obj);
            #endif
            status = true;
            break;
         }
      }
   }

   #if DEBUG_REFDATA_OBJECT > 1
   for (int i=0; i<mNumRefObjects; i++)
   {
      MessageInterface::ShowMessage
         ("type=%d, name=%s, obj=%p\n", mRefObjList[i].objType,
          mRefObjList[i].objName.c_str(), mRefObjList[i].obj);
   }   
   #endif
   
   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("*** Warning *** RefData::SetRefObject() Cannot find type=%s\n",
       GmatBase::GetObjectTypeString(actualType).c_str());
   #endif
   
   return status;
}


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
   
   if (type != Gmat::SPACECRAFT && type != Gmat::COORDINATE_SYSTEM &&
       type != Gmat::CALCULATED_POINT && type != Gmat::BURN) //loj: 7/20/06 Added BURN
      return true;
   
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
         }
      }
   }
   
   return true;
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
// bool AddRefObject(const Gmat::ObjectType type, const std::string &name,
//                   GmatBase *obj = NULL, bool replaceName = false)
//------------------------------------------------------------------------------
/**
 * Adds object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::AddRefObject(const Gmat::ObjectType type, const std::string &name,
                           GmatBase *obj, bool replaceName)
{
   #if DEBUG_REFDATA_ADD
   MessageInterface::ShowMessage
      ("==> RefData::AddRefObject() mNumRefObjects=%d, type=%d, name=%s, obj=%p, "
       "replaceName=%d\n", mNumRefObjects, type, name.c_str(), obj, replaceName);
   #endif
   
   if (IsValidObjectType(type))
   {
      if (FindFirstObjectName(type) == "")
      {
         if (mNumRefObjects < MAX_OBJ_COUNT)
         {
            mRefObjList[mNumRefObjects].objType = type;
            mRefObjList[mNumRefObjects].objName = name;
            mRefObjList[mNumRefObjects].obj = obj;
            mNumRefObjects++;
            
            #if DEBUG_REFDATA_ADD
            MessageInterface::ShowMessage
               ("==> RefData::AddRefObject() object type:%d name:%s added. mNumRefObjects=%d\n",
                type, name.c_str(), mNumRefObjects);
            #endif
            
            return true;
         }
      }
      else
      {
         if (replaceName)
            SetRefObjectWithNewName(obj, type, name);
         else
            SetRefObject(obj, type, name);
         
         return true;
      }
   }
   
   #if DEBUG_REFDATA_ADD
   MessageInterface::ShowMessage("RefData::AddRefObject() return false\n");
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// bool SetRefObjectWithNewName(GmatBase *obj, const Gmat::ObjectType type,
//                              const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets object pointer with new name which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::SetRefObjectWithNewName(GmatBase *obj, const Gmat::ObjectType type,
                                      const std::string &name)
{
   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObjectWithNewName() numRefObjects=%d, type=%d, name=%s "
       "obj addr=%p\n", mNumRefObjects, type, name.c_str(), obj);
   #endif
   
   #if DEBUG_REFDATA_OBJECT > 1
   for (int i=0; i<mNumRefObjects; i++)
   {
      MessageInterface::ShowMessage
         ("type=%d, name=%s, obj=%d\n", mRefObjList[i].objType,
          mRefObjList[i].objName.c_str(), mRefObjList[i].obj);
   }   
   #endif
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         mRefObjList[i].objName = name;
         mRefObjList[i].obj = obj;
         
         #if DEBUG_REFDATA_OBJECT > 1
         MessageInterface::ShowMessage
            ("RefData::SetRefObjectWithName() set %s to %d\n",
             mRefObjList[i].objName.c_str(), obj);
         #endif
         return true;
      }
   }

   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("*** Warning *** RefData::SetRefObjectWithName() Cannot find type=%s\n",
       GmatBase::GetObjectTypeString(type).c_str());
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void RefData::InitializeRefObjects()
{
   //do nothing here
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
// GmatBase* FindFirstObject(const std::string &typeName) const
//------------------------------------------------------------------------------
/**
 * @return first object found for given object type name.
 */
//------------------------------------------------------------------------------
GmatBase* RefData::FindFirstObject(const std::string &typeName) const
{   
   return FindFirstObject(GmatBase::GetObjectType(typeName));
}


//------------------------------------------------------------------------------
// GmatBase* FindFirstObject(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * @return first object found for given object type.
 */
//------------------------------------------------------------------------------
GmatBase* RefData::FindFirstObject(const Gmat::ObjectType type) const
{
   #if DEBUG_REFDATA_FIND
   MessageInterface::ShowMessage
      ("RefData::FindFirstObject() type=%d mNumRefObjects=%d\n",
       type, mNumRefObjects);
   #endif
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      #if DEBUG_REFDATA_FIND
      MessageInterface::ShowMessage
         ("RefData::FindFirstObject() i=%d, type=%d, name=%s, obj=%p\n", i,
          mRefObjList[i].objType, mRefObjList[i].objName.c_str(), mRefObjList[i].obj);
      #endif
      
      if (mRefObjList[i].objType == type)
      {
         #if DEBUG_REFDATA_FIND
         MessageInterface::ShowMessage
            ("RefData::FindFirstObject() returning %p\n", mRefObjList[i].obj);
         #endif
         
         return mRefObjList[i].obj;
      }
   }
   
   #if DEBUG_REFDATA_FIND
   MessageInterface::ShowMessage
      ("RefData::FindFirstObject() returning NULL\n");
   #endif
   
   return NULL;
}


//------------------------------------------------------------------------------
// std::string FindFirstObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * @return first object name found for given object type.
 */
//------------------------------------------------------------------------------
std::string RefData::FindFirstObjectName(const Gmat::ObjectType type) const
{
   for (int i=0; i<mNumRefObjects; i++)
   {
      #if DEBUG_REFDATA_OBJECT > 1
      MessageInterface::ShowMessage
         ("RefData::FindFirstObjectName() mRefObjList[%d].objType=%d, objName=%s\n",
          i, mRefObjList[i].objType, mRefObjList[i].objName.c_str());
      #endif
      
      if (mRefObjList[i].objType == type)
         return mRefObjList[i].objName;
   }
   
   return "";
}

