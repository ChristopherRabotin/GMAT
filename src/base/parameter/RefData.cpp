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
   mNumRefObjects = 0;
   mObjTypeObjMap = new std::map<std::string, GmatBase*>;
}

//------------------------------------------------------------------------------
// RefData(const RefData &data)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <data> the RefData object being copied.
 */
//------------------------------------------------------------------------------
RefData::RefData(const RefData &data)
{
   mNumRefObjects = data.mNumRefObjects;
   mObjTypeObjMap = data.mObjTypeObjMap;
}

//------------------------------------------------------------------------------
// RefData& operator= (const RefData& right)
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
   mNumRefObjects = right.mNumRefObjects;
   mObjTypeObjMap = right.mObjTypeObjMap;
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
   delete mObjTypeObjMap;
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer RefData::GetNumRefObjects() const
{
   return mNumRefObjects;
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const std::string &objType)
//------------------------------------------------------------------------------
GmatBase* RefData::GetRefObject(const std::string &objType)
{
   return FindFirstObject(objType);
}

//------------------------------------------------------------------------------
// bool SetRefObject(Gmat::ObjectType objType, const std::string &objName,
//                   GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Sets object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::SetRefObject(Gmat::ObjectType objType,
                           const std::string &objName, GmatBase *obj)
{
   bool status = false;

   //MessageInterface::ShowMessage("RefData::SetRefObject() objName=%s\n", objName.c_str());
   if (obj->GetType() == objType)
   {

      std::string objTypeName = obj->GetTypeName();

      std::map<std::string, GmatBase*>::iterator pos;
      pos = mObjTypeObjMap->find(objTypeName);
      if (pos != mObjTypeObjMap->end())
      {
         pos->second = obj;
         status = true;
         //MessageInterface::ShowMessage("RefData::SetRefObject() obj set to %s\n",
         //                              obj->GetName().c_str());
      }
   }
    
   return status;
}


//------------------------------------------------------------------------------
// bool AddRefObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::AddRefObject(GmatBase*obj)
{
   bool added;
   //MessageInterface::ShowMessage("RefData::AddRefObject() objType=%s, objName=%s\n",
   //                              obj->GetTypeName().c_str(), obj->GetName().c_str());

   if (IsValidObject(obj))
   {
      std::string objTypeName = obj->GetTypeName();
      if (HasObjectType(objTypeName))
      {
         added = false;
      }
      else
      {
         mObjTypeObjMap->insert(std::pair<std::string, GmatBase*>(objTypeName, obj));
         mNumRefObjects = mObjTypeObjMap->size();
         added = true;
         //MessageInterface::ShowMessage("RefData::AddRefObject() object added\n");

      }
   }
   else
   {
      added = false;
   }

   return added;
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
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void RefData::InitializeRefObjects()
{
}

//------------------------------------------------------------------------------
// bool HasObjectType(const std::string &objType) const
//------------------------------------------------------------------------------
/**
 * @return true if the map has the object, false otherwise
 */
//------------------------------------------------------------------------------
bool RefData::HasObjectType(const std::string &objType) const
{
   bool found = false;
   std::map<std::string, GmatBase*>::iterator pos;
   
   pos = mObjTypeObjMap->find(objType);
   
   if (pos != mObjTypeObjMap->end())
      found = true;
   
   return found;
}

//------------------------------------------------------------------------------
// GmatBase* FindFirstObject(const std::string &objType) const
//------------------------------------------------------------------------------
/**
 * @return forst object found for given type name.
 */
//------------------------------------------------------------------------------
GmatBase* RefData::FindFirstObject(const std::string &objType) const
{
   std::map<std::string, GmatBase*>::iterator pos;
   
   pos = mObjTypeObjMap->find(objType);
   
   if (pos != mObjTypeObjMap->end())
   {
      //MessageInterface::ShowMessage("RefData::FindFirstObject(): objType=%s "
      //                              "objName=%s\n", (pos->second->GetTypeName()).c_str(),
      //                              (pos->second->GetName()).c_str());
      return pos->second;
   }
   
   return NULL;
}

