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

#include <math.h>

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
    mStringObjectMap = new std::map<std::string, GmatBase*>;
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
    mStringObjectMap = data.mStringObjectMap;
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
    mStringObjectMap = right.mStringObjectMap;
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
   delete mStringObjectMap;
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer RefData::GetNumRefObjects() const
{
    return mNumRefObjects;
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const std::string &objTypeName)
//------------------------------------------------------------------------------
GmatBase* RefData::GetRefObject(const std::string &objTypeName)
{
    return FindObject(objTypeName);
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
                           const std::string &objName,
                           GmatBase *obj)
{
    bool status = false;


    if (obj->GetType() == objType)
    {

        std::string objTypeName = obj->GetTypeName();

        std::map<std::string, GmatBase*>::iterator pos;
        pos = mStringObjectMap->find(objTypeName);
        if (pos != mStringObjectMap->end())
        {
            pos->second = obj;
            status = true;
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
    
    if (IsValidObject(obj))
    {
        std::string objTypeName = obj->GetTypeName();
        if (HasObject(objTypeName))
        {
            added = false;
        }
        else
        {
            mStringObjectMap->insert(std::pair<std::string, GmatBase*>(objTypeName, obj));
            mNumRefObjects = mStringObjectMap->size();
            added = true;
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
// void Initialize()
//------------------------------------------------------------------------------
void RefData::Initialize()
{
}

//------------------------------------------------------------------------------
// bool HasObject(const std::string &objTypeName) const
//------------------------------------------------------------------------------
/**
 * @return true if the map has the object, false otherwise
 */
//------------------------------------------------------------------------------
bool RefData::HasObject(const std::string &objTypeName) const
{
    bool found = false;
    std::map<std::string, GmatBase*>::iterator pos;
   
    pos = mStringObjectMap->find(objTypeName);
   
    if (pos != mStringObjectMap->end())
        found = true;
   
    return found;
}

//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &objTypeName) const
//------------------------------------------------------------------------------
/**
 * @return object for given type name.
 */
//------------------------------------------------------------------------------
GmatBase* RefData::FindObject(const std::string &objTypeName) const
{
    std::map<std::string, GmatBase*>::iterator pos;
   
    pos = mStringObjectMap->find(objTypeName);
   
    if (pos != mStringObjectMap->end())
        return pos->second;
   
    return NULL;
}

